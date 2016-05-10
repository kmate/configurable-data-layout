#include <cstdio>

const int NUM_NODES = 4;

struct node
{
    unsigned dist[NUM_NODES];
    unsigned from[NUM_NODES];
};

struct node rt[NUM_NODES];

int main()
{
    int dmat[NUM_NODES][NUM_NODES];
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
