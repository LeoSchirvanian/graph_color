#include "color.h"

int taille_buffer = 64;
int taille_population;

int nb_sommets;
int **graph;
int **population;
int *couleurs;


int lireDonnee(char *chemin){
    FILE *f;
    f=fopen(chemin,"r"); // Ouverture du fichier
    if(f!=NULL){
        char buffer[taille_buffer];
        int sommet1, sommet2 = 0;

		// on skip l'entête et on va à la ligne p
		do fgets(buffer, taille_buffer, f);
		while (buffer[0] != 'p');
		sscanf(buffer, "p edge %d", &nb_sommets);	

		// création matrice d'adjacence
		graph = (int **) malloc(sizeof(int*)*nb_sommets);
        for(int i = 0; i < nb_sommets; i++) graph[i] = (int *) malloc(sizeof(int)*nb_sommets);
        
		while(fscanf(f,"%s %d %d\n",buffer,&sommet1,&sommet2) != -1)
		{        	
           	// printf("\n%s %d %d",buffer, sommet1, sommet2);
		               
            if(buffer[0]=='e')
			{
                graph[sommet1-1][sommet2-1] = 1;
				graph[sommet2-1][sommet1-1] = 1;
            }
        }
        return 1;
    }
	else printf("\nErreur ! Impossible d'ouvrir le fichier !\n"); 
	return 0;   
}


void affiche(){
    for(int i = 0;i<nb_sommets;i++) printf("%d : %d\n",i,couleurs[i]);
}


void enregistre(char* chemin){
    FILE* fichier = NULL;
    fichier = fopen(chemin, "w");
     if (fichier != NULL)
    {
        for(int i = 0;i<nb_sommets;i++) fprintf(fichier,"%d\n",couleurs[i]);
        fclose(fichier);
    }   
}

//=============================================================



//=================================================================


void createPopulation(int max_color, int nb_individu)
{
	taille_population = nb_individu;
	
	// creation tableau population [taille_population][nb_sommets]
	population = (int **) malloc(sizeof(int *)*taille_population);
	for(int i=0; i < taille_population; i++)
	{
		population[i] = (int *) malloc(sizeof(int)*nb_sommets);
	}
	
	// génération population random
	for(int i=0; i < taille_population; i++)
	{
		for(int j=0; j < nb_sommets; j++)
		{
			population[i][j] = rand() / (RAND_MAX / max_color+1);
		}
	}
}

int check_node(int i, int indiv){
    for(int j = 0; j<nb_sommets; j++){
        if(graph[i][j]==1 && population[indiv][i] == population[indiv][j])
        {
            //printf("Conflit : Sommet n°%d (couleur : %d) <-> Sommet n°%d (couleur %d)\n",i,population[indiv][i],j,population[indiv][j]);
            return 0;
        }
    }
    return 1;
}

int check_solution(int indiv){
    int errors = 0;
    for(int i = 0; i<nb_sommets; i++) if(!check_node(i, indiv)) errors++;
    return errors;
}


void clone(int nb_to_clone)
{
	int liste_conflit[taille_population];
	
	// calcul somme conflit pour chaque individu
	for(int i=0; i < taille_population; i++)
	{
		liste_conflit[i] = check_solution(i);
		// printf("%d ", liste_conflit[i]);
	}
	
	// on garde les indices des 'nb_to_clone' meilleurs individus 
	int memory[nb_to_clone][nb_sommets];
	int pointer=0;
	
	for(int i=0; i < nb_to_clone; i++)
	{
		int temp = liste_conflit[0];
		int best_index;
		
		for(int j=0; j < taille_population; j++)
		{	
			if(liste_conflit[j] <= temp) 
			{
				temp = liste_conflit[j];			
				best_index = j;						
			}
		}
		// ajoute les meilleurs individus à la memoire
		for(int j=0; j < nb_sommets; j++)
		{
			memory[pointer][j] = population[best_index][j];
		}
		pointer++;
		// on sature la valeur de conflit pour les indiv déjà ajoutés pour ne plus les prendre en compte
		liste_conflit[best_index] = nb_sommets+1;
	}

/* 	printf("\n");
	for(int i = 0; i<taille_population; i++)
	{
		printf("%d ", liste_conflit[i]);
	} 
	printf("\n");
	for(int i = 0; i<nb_to_clone; i++)
	{
		printf("%d ", memory[i]);
	}
	printf("\n");
	for(int i = 0; i<nb_to_clone; i++)
	{
		for(int j = 0; j<nb_sommets; j++)
		{
			printf("%d ", memory[i][j]);
		}
		printf("\n");
	} */

	// on clone les meilleurs individus dans population
	
	pointer = 0;
	for(int i=0; i < taille_population; i++)
	{
		for(int j=0; j < nb_sommets; j++)
		{
			if(pointer < nb_to_clone) 
			{
				population[i][j] = memory[pointer][j];
			}
		}
		pointer++;
		if(pointer >= nb_to_clone) 
			{
				pointer = 0;
			}
	}
}





//======================================================================

int main(int argc, char* args[]) 
{
    if(argc>=1)
	{
    	// read input file and create graph
        if(lireDonnee(args[1]))
		{
        //_____________génétique__________________
        	
        	createPopulation(100, 5);
        	
        	clone(2);
            
            return 1;
        }
    }
	else
	{
        printf("Vous devez donner un fichier en argument !\n");
        return 0;
    }
}
