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


void enregistre(char* chemin, int best_indiv){
    FILE* fichier = NULL;
    fichier = fopen(chemin, "w");
     if (fichier != NULL)
    {
        for(int i = 0;i<nb_sommets;i++) fprintf(fichier,"%d\n",population[best_indiv][i]);
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
        if(graph[i][j]==1)
        {
			if( population[indiv][i] == population[indiv][j])
			{
				return 0;
			}
            //printf("Conflit : Sommet n°%d (couleur : %d) <-> Sommet n°%d (couleur %d)\n",i,population[indiv][i],j,population[indiv][j]);
        }
    }
    return 1;
}

//Calcule le nombre d'erreurs pour un individu donné
int check_solution(int indiv){
    int errors = 0;
    for(int i = 0; i<nb_sommets; i++) if(check_node(i, indiv)==0) errors++;
    return errors;
}


int get_color(int indiv){
	// renvoie la plus grande couleur de l'individu
	int max = 0;
	for(int i =0; i<nb_sommets; i++){
		if(population[indiv][i] > max){
			max = population[indiv][i];
		}
	}
	return(max);
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
		int best_index = 0;

		
		for(int j=0; j < taille_population; j++)
		{	
			if(liste_conflit[j] < temp) 
			{
				temp = liste_conflit[j];		
				best_index = j;						
			}			
			if(liste_conflit[j] == temp){
				if(get_color(j) <= get_color(best_index))
				{
					best_index = j;
				}				
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
	{int get_color(int indiv){
	int max = 0;
	for(int i =0; i<nb_sommets; i++){
		if(population[indiv][i] > max){
			max = population[indiv][i];
		}
	}
	return(max);
}
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



int evaluate(){
	// permet de récupérer le meilleur individu valide
	int best = -1;
	int color = nb_sommets+1;
	for(int i = 0; i<taille_population; i++){
		if( check_solution(i) == 0){
			int temp = get_color(i);
			if( temp < color){
				best = i;
				color = temp;
			}
		}
	}
	// affiche erreur si aucun individu valide
	if(best == -1)
	{
		printf("\naucun résultat satisfaisant\n");
		return 0;
	}

	for(int i = 0; i <nb_sommets; i++){
		printf("%d  ",population[best][i]);
	}
	enregistre("/home/sebastien/Documents/Cours/ecole_des_mines/3A/IA/metaheuristic/TP_genetic/results/res1", best);
}

void mutate(float indiv_mutation_proba, float gene_mutation_proba)
{
	// Chaque individue à une probabilité de muter mais on ne fait pas muter ceux qui sont valides
	for(int i=0; i < taille_population; i++)
	{
		if(rand()/RAND_MAX <= indiv_mutation_proba && check_solution(i) != 0)
		{
			// Si l'individue doit muter alors chaque gène à une proba de muter
			for(int j=0; j< nb_sommets; j++)
			{
				if(rand()/RAND_MAX <= gene_mutation_proba)
				{
					/* quand le gène mute il a 50% de chance de switcher avec un gène au pif
					et 50% diminuer sa couleur de x, x in [0, 10] */
					if( rand() / (RAND_MAX/2)>0)
					{
						int temp0 = population[i][j];
						int temp1 = rand() / (RAND_MAX/nb_sommets);
						population[i][j] = population[i][temp1];
						population[i][temp1] = temp0;
					}else
					{
						int temp2 = rand() / (RAND_MAX/10);
						if(population[i][j]-temp2 >= 0) population[i][j] -= temp2;
					}				
					//population[i][j] = population[i][j] - 1; 
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
        //_____________génétique___and()/RAND_MAX) * nb_somm_______________
        	
        	createPopulation(100, 100);
            
			do{
				start = clock();

				clone(20);

				mutate(0.4, 0.2);

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
