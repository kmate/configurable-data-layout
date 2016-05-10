-- Configuration
data ConfigItem = Scalar
                | Ptr ConfigItem
                | Array ConfigItem

data ConfigToken = SCALAR | PTR | ARRAY
    deriving Show

type Config = [ConfigToken]
type Dimensions = [Int]

config :: ConfigItem -> Config
config Scalar = [SCALAR]
config (Ptr c) = PTR : (config c)
config (Array c) = ARRAY : (config c)

-- Item configuration
itemConfig:: Config -> Config
itemConfig c = fst $ foldl itemConfig' ([], False) c
    where
        itemConfig' (cfg, add) SCALAR = (if add then cfg ++ [SCALAR] else cfg, add)
        itemConfig' (cfg, add) PTR    = (if add then cfg ++ [PTR] else cfg, add)
        itemConfig' (cfg, add) ARRAY  = (if add then cfg ++ [ARRAY] else cfg, True)

-- Item configuration pointer checking
hasPtrItem :: Config -> Bool
hasPtrItem c = fst $ foldl hasPtrItem' (False, False) c
    where
        hasPtrItem' (hpr, inar) SCALAR = (hpr, inar)
        hasPtrItem' (hpr, inar) PTR    = (if inar then True else hpr, inar)
        hasPtrItem' (hpr, inar) ARRAY  = (hpr, True)

-- Array rank (the number of dimensions) of a configuration
arrayRank :: Config -> Int
arrayRank c = foldl arrayRank' 0 c
    where
        arrayRank' cd SCALAR = cd
        arrayRank' cd PTR    = cd
        arrayRank' cd ARRAY  = cd + 1


-- Helper types and functions
type BaseType = String
type Code = String
type Identifier = String
type Indexes = [String]

parens :: Identifier -> Code
parens id = "(" ++ id ++ ")"


-- Library configuration
allocateFunction :: Code
allocateFunction = "malloc"

deallocateFunction :: Code
deallocateFunction = "free"


-- Declaration
-- these are also usable for struct members
declare :: BaseType -> Identifier -> Config -> Code
declare bt id c | arrayRank c > 0 = error "For array configurations use declareArray!"
                | otherwise             = bt ++ " " ++ declare'
    where
        declare' = foldl applyToken "" c
        applyToken code SCALAR = code ++ id
        applyToken code PTR    = '*' : code

declareArray :: BaseType -> Identifier -> Config -> Dimensions -> Code
declareArray bt id c dim | arrayRank c > length dim = error "Please specify the size of all array dimensions for declareArray!"
                         | arrayRank c < length dim = error "Too many dimension sizes specified for declareArray!"
                         | otherwise                = bt ++ " " ++ fst declare'
    where
        declare' = foldl applyToken ("", (True, dim)) c
        applyToken (code, (aid, dim)) SCALAR = (code, (aid, dim))
        applyToken (code, (aid, dim)) PTR    = ("(*" ++ code ++ (addId aid) ++ ")", (False, dim))
        applyToken (code, (aid, dim)) ARRAY  = ('(' : code ++ (addId aid) ++ "[" ++ (show $ head dim) ++ "])", (False, tail dim))
        addId aid = if aid then id else ""

-- use them in function prototypes
declareFunction :: BaseType -> Identifier -> Code -> Config -> Code
declareFunction bt id args c = bt ++ " " ++ declareFunction'
    where
        declareFunction' = foldl applyToken "" c
        applyToken code SCALAR = code ++ id ++ parens args
        applyToken code PTR    = '*' : code
        applyToken code ARRAY  = '*' : code

argument :: BaseType -> Identifier -> Config -> Code
argument = declare

arrayArgument :: BaseType -> Identifier -> Config -> Dimensions -> Code
arrayArgument = declareArray


-- Allocation
allocate :: Identifier -> Config -> Code
allocate id c = "do {" ++ allocate' ++ "} while(0)"
    where
        (allocate', _, _, _, _, _) = foldl applyToken ("", "", "", id, 0, c) c
        applyToken (code, l, r, id, d, c) SCALAR = (l ++ r, l, r, id, d, c)
        applyToken (code, l, r, id, d, c) PTR    =
            ( code
            , l ++ id ++ " = " ++ allocateFunction ++ "(sizeof(*(" ++ id ++ ")));"
            , r
            , '*' : parens id
            , d
            , c
            )
        applyToken (code, l, r, id, d, c) ARRAY  =
            ( code
            , if hasPtrItem c
              then l ++ "for(int i" ++ show d ++ " = 0; i" ++ show d ++
                " < sizeof(" ++ id ++ ") / sizeof((" ++ id ++ ")[0]); ++i" ++ show d ++ ") {"
              else l
            , if hasPtrItem c then '}' : r else r
            , parens id ++ "[i" ++ show d ++ "]"
            , d + 1
            , itemConfig c
            )


-- Deallocation
deallocate :: Identifier -> Config -> Code
deallocate id c = "do {" ++ deallocate' ++ "} while(0)"
    where
        (deallocate', _, _, _, _, _) = foldl applyToken ("", "", "", id, 0, c) c
        applyToken (code, l, r, id, d, c) SCALAR = (l ++ r, l, r, id, d, c)
        applyToken (code, l, r, id, d, c) PTR    =
            ( code
            , l
            , deallocateFunction ++ (parens id) ++ ";" ++ r
            , '*' : parens id
            , d
            , c
            )
        applyToken (code, l, r, id, d, c) ARRAY  =
            ( code
            , if hasPtrItem c 
              then l ++ "for(int i" ++ show d ++ " = 0; i" ++ show d ++
                " < sizeof(" ++ id ++ ") / sizeof((" ++ id ++ ")[0]); ++i" ++ show d ++ ") {"
              else l
            , if hasPtrItem c then '}' : r else r
            , parens id ++ "[i" ++ show d ++ "]"
            , d + 1
            , itemConfig c
            )


-- Value of
valueOf :: Identifier -> Config -> Code
valueOf id c = fst $ foldl applyToken ("", True) c
    where
        applyToken (code, apr) SCALAR = (code ++ (parens id), False)
        applyToken (code, apr) PTR    = (if apr then '*' : code else code, apr)
        applyToken (code, apr) ARRAY  = (code, False)


-- Value at a specified array index
valueAt :: Identifier -> Config -> Indexes -> Code
valueAt id c idx | 0 == length idx          = error "Please specify at least one index for valueAt!"
                 | arrayRank c < length idx = error "Too many indexes specified for valueAt!"
                 | otherwise                = fst $ foldl valueAt' (valueOf id c, itemConfig c) idx
    where
        valueAt' (id, cfg) i = (valueOf id cfg ++ "[" ++ i ++ "]", itemConfig cfg)


-- Structure member access
memberOf :: Identifier -> Config -> Identifier -> Code
memberOf obj objc member = "(" ++ valueOf obj objc ++ ")." ++ member


-- Test configurations
c_s = config(Scalar)
c_ps = config(Ptr(Scalar))
c_pps = config(Ptr(Ptr(Scalar)))
c_ppps = config(Ptr(Ptr(Ptr(Scalar))))
c_as = config(Array(Scalar))
c_pas = config(Ptr(Array(Scalar)))
c_ppas = config(Ptr(Ptr(Array(Scalar))))
c_aps = config(Array(Ptr(Scalar)))
c_apps = config(Array(Ptr(Ptr(Scalar))))
c_paps = config(Ptr(Array(Ptr(Scalar))))
c_papps = config(Ptr(Array(Ptr(Ptr(Scalar)))))
c_ppaps = config(Ptr(Ptr(Array(Ptr(Scalar)))))
c_ppapps = config(Ptr(Ptr(Array(Ptr(Ptr(Scalar))))))
c_aas = config(Array(Array(Scalar)))
c_paas = config(Ptr(Array(Array(Scalar))))
c_apas = config(Array(Ptr(Array(Scalar))))
c_aaps = config(Array(Array(Ptr(Scalar))))
c_apaps = config(Array(Ptr(Array(Ptr(Scalar)))))
c_papas = config(Ptr(Array(Ptr(Array(Scalar)))))
c_papaps = config(Ptr(Array(Ptr(Array(Ptr(Scalar))))))
c_ppappapps = config(Ptr(Ptr(Array(Ptr(Ptr(Array(Ptr(Ptr(Scalar)))))))))
