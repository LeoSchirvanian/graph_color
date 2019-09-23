#include <stdio.h>

int lirePb(char* nom);
int lireSol(char* nom);
void chkColor(void);

int main(int n,char **a)
	{
	if (n<3) return 0;	
	if (lirePb(a[1])==0)
		if (lireSol(a[2])==0)
			chkColor();
	}

#define MAX_SOMMETS 1000
int nb_sommets;
int adjacent[MAX_SOMMETS][MAX_SOMMETS];
int color[MAX_SOMMETS];

int lirePb(char* nom)
	{
	int i,j;
	char buffer[1000];
	FILE* f=fopen(nom,"r");
	if (f==NULL){printf("\techec en ouverture du fichier %s\n",nom);return 1;}
	do fgets(buffer,1000,f);while(buffer[0]!='p');
	sscanf(buffer,"p edge %d",&nb_sommets);
	printf("\n%s %d sommets",nom,nb_sommets);	
	for(i=0;i<nb_sommets;i++) 
		for(j=0;j<nb_sommets;j++)
			adjacent[i][j]=adjacent[j][i]=0;
	while(fscanf(f,"e %d %d \n",&i,&j)==2)
		{
		if(i<1 || i>nb_sommets) 
			{
			printf("\terreur lecture sommet %d \n",i);
			fclose(f);
			return 2;
			}
		i--;
		if(j<1 || j>nb_sommets) 
			{
			printf("\terreur lecture sommet %d \n",j);
			fclose(f);
			return 2;
			}
		j--;
		adjacent[i][j]=adjacent[j][i]=1;
		}
	fclose(f);
	return 0;
	}

int lireSol(char* nom)
	{
	int s;
	FILE* f=fopen(nom,"r");
	if(f!=NULL)
		{
		for(s=0;s<nb_sommets;s++)
			fscanf(f,"%d",color+s);
		fclose(f);
		return 0;
		}
	printf("\nechec en ouverture du fichier %s\n",nom);
	return 1;
	}

void chkColor(void)
	{
	int i,j,err,k,kmax;
	err=0;
	kmax=0;
	for(i=0;i<nb_sommets-1;i++)
		{
		k=color[i];
		if (k>kmax) kmax=k;
		for(j=i+1;j<nb_sommets;j++)
			if (adjacent[i][j] && color[j]==k)
				{
				printf("\nconflit sommets : %d %d couleur %d\n",i,j,k);
				err++;
				}
		}
	if (err==0)
		{
		if (color[i]>kmax) kmax=color[i];
		printf("\nmajorant %d",kmax);
		return;
		}
	}
