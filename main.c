#include "color.h"

int taille_buffer = 64;
int taille_population;

int nb_sommets;
int **graph;
int **population;
int *couleurs;

#define TIME 60.0 //temps de calcul CPU


//Fonction qui permet de lire les données en entrée
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

//Fonction qui check si les sommets d'un individu ne possèdent pas des voisins de même couleur
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

//Calcule le nombre d'erreurs pour un individu donné
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

int get_color(int indiv){
	int max = 0;
	for(int i =0; i<nb_sommets; i++){
		if(population[indiv][i] > max){
			max = population[indiv][i];
		}
	}
	return(max);
}

int evaluate(){
	int best = 0;
	int color = nb_sommets+1;
	for(int i = 0; i<taille_population; i++){
		if( check_solution(population[i]) == 0){
			if( get_color(population[i]) < color){
				best = i;
			}
		}
	}

	for(int i = 0; i <taille_population; i++){
		printf("%d",population[best][i]);
	} 
}

void mutate(float indiv_mutation_proba, float gene_mutation_proba)
{
	// Chaque individue à une probabilité de muter
	for(int i=0; i < taille_population; i++)
	{
		if(rand()/RAND_MAX <= indiv_mutation_proba)
		{
			// Si l'individue doit muter alors chaque gène à une proba de muter
			for(int j=0; j< nb_sommets; j++)
			{
				if(rand()/RAND_MAX <= gene_mutation_proba)
				{
					// si le gène mute on lui donne un entier aléatoire entre 0 et nb_sommets
					population[i][j] = (rand()/RAND_MAX) * nb_sommets;
				}
			}
		}		
	}
}


//======================================================================

int main(int argc, char* args[]) {

	//Calcul temps CPU
    clock_t start, end;
    double cpu_time_used = 0.0;

    if(argc>=1)
	{
    	// read input file and create graph
        if(lireDonnee(args[1]))
		{
        //_____________génétique__________________
        	
        	createPopulation(100, 1000);
            
			do{
				start = clock();

				clone(50);

				mutate(0.05, 0.04);

				end = clock();
        		cpu_time_used += ((double) (end - start)) / CLOCKS_PER_SEC;

			}while( (cpu_time_used < TIME));

			evaluate();
        }
		return 1;
    }
	else
	{
        printf("Vous devez donner un fichier en argument !\n");
        return 0;
    }
}
