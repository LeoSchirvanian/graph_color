#include "color.h"

int lignes_max_lues = 524288;
int taille_buffer = 64;

int nb_sommets = 0;
int **graph;
int *DSAT;
int *couleurs;
int *degre;

int lireDonnee(char *chemin){
    FILE *f;
    f=fopen(chemin,"r"); // Ouverture du fichier
    if(f!=NULL){
        char buffer[taille_buffer];
        int sommet1,sommet2,nb_arretes = 0;;
        graph = (int **) malloc(sizeof(int*)*lignes_max_lues);
        for(int i = 0;i<lignes_max_lues;i++) graph[i] = (int *) malloc(sizeof(int)*lignes_max_lues);
        while(fscanf(f,"%s %d %d\n",buffer,&sommet1,&sommet2) != -1){
            if(buffer[0]=='e' && buffer[1]==0){
                graph[sommet1-1][sommet2-1] = 1;graph[sommet2-1][sommet1-1] = 1; nb_arretes++;
                if(sommet1>nb_sommets) nb_sommets = sommet1;
                if(sommet2>nb_sommets) nb_sommets = sommet2;
            }
        }
        printf("Fichier ouvert : %s (%d sommets et %d arrêtes)\n",chemin,nb_sommets,nb_arretes); return 1;
    }else printf("\nErreur ! Impossible d'ouvrir le fichier !\n"); return 0;   
}

void update_node(int i){ // Actualise le degre et DSAT d'un noeux :
    int nb_voisins_colorie = 0, nb_voisins = 0;
    for(int j = 0;j<nb_sommets;j++){
        if(graph[i][j] == 1 && i!=j){
            nb_voisins++;
            if(couleurs[j]!=0) nb_voisins_colorie++;
        }
    }
    degre[i] = nb_voisins;
    if(nb_voisins_colorie==0) DSAT[i] = nb_voisins;
    else DSAT[i] = nb_voisins_colorie;
}

void init_arrays(){ // Initialise les trois tableaux suivants :
    couleurs = (int*) malloc(sizeof(int*)*nb_sommets);
    DSAT = (int*) malloc(sizeof(int*)*nb_sommets);
    degre = (int*) malloc(sizeof(int*)*nb_sommets);
    for(int i = 0;i<nb_sommets;i++){
        couleurs[i] = 0;
        update_node(i);
    }
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

int couleur_max(){ // Renvoi l'indice de la plus haute couleur :
    int maxi = 0;
    for(int i = 0;i<nb_sommets;i++) if(maxi<couleurs[i]) maxi = couleurs[i];
    return maxi;
}

int choix(){ // Renvoi l'indice du noeu suivant à traiter
    int indice_max = 0, max = 0;
    for(int i = 0;i<nb_sommets;i++){
        if(couleurs[i]==0){
            if(DSAT[i]==max){
                if(degre[i]>degre[indice_max]){
                    max = DSAT[i];
                    indice_max = i;
                }
            }else if(DSAT[i]>max){
                max = DSAT[i];
                indice_max = i;
            }
        }
    }
    return indice_max;
}

void colorier(int indice){
    int couleur = 0, instances; // Couleur que nous allons attribuer, Compteur de noeux ayant déja la même couleur
    do{
        couleur++;instances=0;
        for(int i = 0;(i<nb_sommets && instances==0);i++) if(graph[indice][i] == 1 && couleur==couleurs[i]) instances++;
    }while(instances!=0);
    couleurs[indice] = couleur;
}

int check_node(int i){
    for(int j = 0;j<nb_sommets;j++){
        if(graph[i][j]==1 && couleurs[i]==couleurs[j]){
            //printf("Conflit : Sommet n°%d (couleur : %d) <-> Sommet n°%d (couleur %d)\n",i,couleurs[i],j,couleurs[j]);
            return 0;
        }
    }
    return 1;
}

int check_solution(){
    int errors = 0;
    for(int i = 0;i<nb_sommets;i++) if(!check_node(i)) errors++;
    return errors;
}

void randomize(int nb_variables,int cMax){
    for(int k = 0;k<nb_variables;k++){
        int s = rand()/(RAND_MAX/(nb_sommets));
        int color = rand()/(RAND_MAX/(cMax));
        couleurs[s] = color+1;
    }
}

void recuit(int result){
    float T = 0.5;
    float Tmin = 0.001;
    float a = 0.99;
    int iterMax = 100;
    int *oldColors = (int*) malloc(sizeof(int)*nb_sommets);
    while(T>Tmin){
        int cMax = couleur_max();
        for(int i = 0;i<iterMax;i++){
            for(int i = 0;i<nb_sommets;i++){
                oldColors[i] = couleurs[i];
            }
            randomize((int)(T*20+1),cMax-1+10*T);
            int newResult = 10*check_solution()+couleur_max();
            if (newResult>result){
                float p = rand()/((float)(RAND_MAX));
                if(p>exp(-((float)(newResult-result))/T)){
                    //printf("-");
                    for(int i = 0;i<nb_sommets;i++){
                        couleurs[i] = oldColors[i];
                    }
                }else{
                    result = newResult;
                }
            }else{
                //printf("Réduction de Conflits : %d , Couleurs : %d\n",check_solution(),couleur_max());
                result = newResult;
            }
        }
        printf("T = %2.4f : (Conflits : %d , Couleurs : %d)\n",T,check_solution(),couleur_max());
        T = T*a;
    }
}

int main(int argc, char* args[]) {
    if(argc>=2){
        if(lireDonnee(args[1])){
            init_arrays();
            int indice;
            for(int i = 0;i<nb_sommets;i++){
                indice = choix();
                colorier(indice);
                update_node(indice);
            }
            int result = check_solution();
            printf("DSATUR : Conflits : %d , Couleurs : %d\n",result,couleur_max());
            
            //check_solution(1);
            randomize(10,couleur_max());
            
            
            // RECUIT :
            result = check_solution();
            recuit(10*result+couleur_max());
            
            affiche();
            printf("RECUIT : Conflits : %d , Couleurs : %d\n",check_solution(),couleur_max());
            if(argc>=2){
                enregistre(args[2]);
            }
            return 1;
        }
    }else{
        printf("Vous devez donner un fichier en argument !\n");
        return 0;
    }
    
}
