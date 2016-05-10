#include <cstdio>
#include "template_config.hpp"

using namespace cfg;

#define CONFIG_MINIMAL // configuration switch

#ifdef CONFIG_MINIMAL // this is the "minimal" working configuration for this code
    typedef ARRAY<SCALAR> dist_config;
    typedef ARRAY<SCALAR> from_config;
    typedef ARRAY<ARRAY<SCALAR>> dmat_config;
#else
    typedef PTR<ARRAY<SCALAR>> dist_config;
    typedef PTR<ARRAY<SCALAR>> from_config;
    typedef ARRAY<PTR<ARRAY<SCALAR>>> dmat_config;
#endif



const int NUM_NODES = 4;

struct node
{
    configured_array<unsigned, dist_config, true, NUM_NODES> dist;
    configured_array<unsigned, from_config, true, NUM_NODES> from;
};

struct node rt[NUM_NODES];

int main()
{
    configured_array<int, dmat_config, false, NUM_NODES, NUM_NODES> dmat;
    dmat.allocate();
    int i,j,k,count=0;

    for(i=0;i<NUM_NODES;i++) {
        for(j=0;j<NUM_NODES;j++) {
            scanf("%d",&dmat[i][j]);
            dmat[i][i] = 0;
            rt[i].dist[j] = dmat[i][j];
            rt[i].from[j] = j;
        }
    }
    do {
        count=0;
        for(i=0;i<NUM_NODES;i++) {
            for(j=0;j<NUM_NODES;j++) {
                for(k=0;k<NUM_NODES;k++) {
                    if( rt[i].dist[j] > dmat[i][k] + rt[k].dist[j]) {
                        rt[i].dist[j] = rt[i].dist[k] + rt[k].dist[j];
                        rt[i].from[j] = k;
                        count++;
                    }
                }
            }
        }
    } while(count!=0);
    dmat.deallocate();
    for(i=0;i<NUM_NODES;i++)
    {
        printf("\n\nState value for router %d is \n",i+1);
        for(j=0;j<NUM_NODES;j++)
        {
            printf("\tnode %d via %d [distance %d]\n",j+1,rt[i].from[j]+1,rt[i].dist[j]);
        }
    }
    printf("\n\n");
}
