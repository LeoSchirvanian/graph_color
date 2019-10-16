#include "color.h"
#include <string.h>
#include <time.h>

#define MAX_TIME_SOLVE_SEC 60

int lignes_max_lues = 524288;
int taille_buffer = 64;

int nb_sommets = 0;
int **graph;
int *DSAT;
int *couleurs;
int *degre;

int *nb_voisins;
int **adj;

int *rang;

int *memory2D;

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

void init_adj(){
    adj = (int**) malloc(sizeof(int*)*nb_sommets);
    nb_voisins = (int*) malloc(sizeof(int)*nb_sommets);
    for(int i = 0;i<nb_sommets;i++){
        nb_voisins[i] = 0;
        adj[i] = (int*) malloc(sizeof(int)*nb_sommets);
        for(int j = 0;j<nb_sommets;j++){
            if(graph[i][j]) adj[i][nb_voisins[i]++]=(int) j;
        }
    }
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

int check_gen_node(int* gen, int i){
    for(int j = 0;j<nb_voisins[i];j++){
        if(gen[i]==gen[adj[i][j]]){
            return 0;
        }
    }
    return 1;
}

int score_indiv(int* gen){
    int c= 0;
    for(int i = 0;i<nb_sommets;i++){
        for(int j = 0;j<nb_voisins[i];j++){
            c+=(gen[i]==gen[adj[i][j]]);
        }
    }
    return c;
}

int check_gen_confilts(int* gen){
    int errors = 0;
    for(int i = 0;i<nb_sommets;i++){
        errors+=check_gen_node(gen,i);
    }
    return errors;
}

int check_gen_colors(int* gen){
    int maxColor = 0;
    for(int i = 0;i<nb_sommets;i++){
        if(maxColor<gen[i]) maxColor = gen[i];
    }
    return maxColor;
}

void random_gen(int* gen, int maxColor){
    for(int i = 0;i<nb_sommets;i++){
        gen[i] = rand()/(RAND_MAX/(maxColor));
    }
}

void sort_gens(int** gens, int* scores, int pop){
    int errors;
    do{
        errors = 0;
        for(int i = 0;i<pop-1;i++){
            if(scores[i]>scores[i+1]){
                errors+=1;
                int* buff = malloc(sizeof(int)*nb_sommets);
                int score = scores[i+1];
                scores[i+1] = scores[i];
                scores[i] = score;
                for(int j = 0;j<nb_sommets;j++){
                    buff[j] = gens[i+1][j];
                    gens[i+1][j] = gens[i][j];
                    gens[i][j] = buff[j];
                }
            }
        }
    }while(errors>0);
}

void cross(int** gens,int id, int id1, int id2, float mut,float mut2, int maxColor){
    int delta = 0;
    int cut = rand()/(RAND_MAX/(nb_sommets));
    for(int i = 0;i<cut;i++){
        if((float)rand()/(RAND_MAX)<mut){
            int c = 0; int ok = 1;
            do{
                c++;
                ok = 1;
                for(int kv = 0; kv<nb_voisins[i];kv++){
                    if((float)rand()/(RAND_MAX)<(mut2) || c==gens[id][i] || c==gens[id1][adj[i][kv]]){
                         ok = 0; break;
                    }
                }
            }while(!ok);
            gens[id][i]=c;
        }else{
            gens[id][i] = gens[id1][i];
        }
    }
    for(int i = cut;i<nb_sommets;i++){
        if((float)rand()/(RAND_MAX)<mut){
            int c = 0; int ok = 1;
            do{
                c++;
                ok = 1;
                for(int kv = 0; kv<nb_voisins[i];kv++){
                    if((float)rand()/(RAND_MAX)<(mut2) || c==gens[id][i] || c==gens[id2][adj[i][kv]]){
                         ok = 0; break;
                    }
                }
            }while(!ok);
            gens[id][i]=c;
        }else{
            gens[id][i] = gens[id2][i];
        }
    }
    
}

void init_tri(){
    memory2D = malloc(sizeof(int)*nb_sommets);
}

void quickSort2(int* arrayS,int* array, int leftIndex, int rightIndex)
{
    // allow to sort in increasing order array allong a column
    if(leftIndex >= rightIndex) return;
    // pivot choice
    int pivot = array[rightIndex];
    // pointer
    int pointer = leftIndex;
    for(int i = leftIndex; i <= rightIndex; i++)
    {
        if(array[i] <= pivot)
        {
            //swap pointed and the one >= to the pivot
            int memory = array[i];
            array[i] = array[pointer];
            array[pointer] = memory;
            
            memory = arrayS[i];
            arrayS[i] = arrayS[pointer];
            arrayS[pointer] = memory;
            
            pointer++;
        }
    }
    quickSort2(arrayS, array, leftIndex, pointer-2);
    quickSort2(arrayS, array, pointer, rightIndex);
}

void quickSort(int** array2D,int* array, int leftIndex, int rightIndex)
{
    // allow to sort in increasing order array allong a column
    if(leftIndex >= rightIndex) return;
    // pivot choice
    int pivot = array[rightIndex];
    // pointer
    int pointer = leftIndex;
    for(int i = leftIndex; i <= rightIndex; i++)
    {
        if(array[i] <= pivot)
        {
            //swap pointed and the one >= to the pivot
            int memory = array[i];
            array[i] = array[pointer];
            array[pointer] = memory;
            
            for(int k = 0;k<nb_sommets;k++){
                int buff = array2D[pointer][k];
                array2D[pointer][k] = array2D[i][k];
                array2D[i][k] = buff;
            }
            
            pointer++;
        }
    }
    quickSort(array2D, array, leftIndex, pointer-2);
    quickSort(array2D, array, pointer, rightIndex);
}

void genetic(int pop, int sel, int maxIter){
    
    pop--;
    
    int** gens = (int**) malloc(sizeof(int*) * pop);
    int* scores = (int*) malloc(sizeof(int) * pop);
    
    rang = (int*) malloc(sizeof(int)*pop);
    for(int i = 0;i<pop;i++){
        rang[i] = i;
    }
    
    for(int i = 0;i<pop+1;i++){
        gens[i] = (int*) malloc(sizeof(int) * nb_sommets);
        random_gen(gens[i],nb_sommets);
        scores[i] = check_gen_confilts(gens[i])+check_gen_colors(gens[i]);
    }
    printf("%d\n",nb_sommets);
    init_tri();
    quickSort2(rang,scores,0,pop-1);
    
    for(int iter = 0;iter<maxIter;iter++){
        int couleur_max = check_gen_colors(gens[rang[0]]);
        for(int i = 0;i<sel;i++){
            scores[i] += rand()/(RAND_MAX/4);
        }
        for(int i = sel;i<pop;i++){
            int id1 = rand()/(RAND_MAX/(sel));
            int id2 = rand()/(RAND_MAX/(sel));
            cross(gens,rang[i],rang[id1],rang[id2],0.001*(300-sel),0.002f,couleur_max);
            scores[i] = score_indiv(gens[rang[i]]) +((800-sel)/100)*check_gen_colors(gens[rang[i]]);
        }
        quickSort2(rang,scores,0,pop-1);
        
        if(score_indiv(gens[pop])>score_indiv(gens[rang[0]]) || check_gen_colors(gens[pop])>check_gen_colors(gens[rang[0]])){
            printf("%d GENETIC : Conflits : %d (%d), Couleurs : %d, (%d) \n",sel,score_indiv(gens[rang[0]]),score_indiv(gens[pop]), check_gen_colors(gens[rang[0]]),check_gen_colors(gens[pop]));
            for(int k =0;k<nb_sommets;k++){
                gens[pop][k] = gens[rang[0]][k];
            }
        }
       sel+=-1;
        if(sel<100){
            sel = 300;
        }
        
    }
}

void recuit(int result){
    int clock_last = clock();
    int clock_first = clock_last;
    float T = 0.06;
    float Tmin = 0.00001;
    float a = 0.99;
    int iterMax = 200000;
    int *oldColors = (int*) malloc(sizeof(int)*nb_sommets);
    
    while(T>Tmin && (clock()-clock_first)/CLOCKS_PER_SEC<MAX_TIME_SOLVE_SEC){
        int cMax = couleur_max();
        for(int i = 0;i<iterMax;i++){
            /*for(int i = 0;i<nb_sommets;i++) oldColors[i] = couleurs[i];
            int newResult = randomize(1,cMax,result);
            if (newResult>result){
                float p = rand()/((float)(RAND_MAX));
                if(p>exp(-((float)(newResult-result))/T)){ 
                    for(int i = 0;i<nb_sommets;i++) couleurs[i] = oldColors[i];
                }else result = newResult;
            }else result = newResult;*/
            //randomize(cMax,T);
        }
        if((clock()-clock_last)/CLOCKS_PER_SEC>=1){
            printf("%2.lds -> T = %2.4f : (Conflits : %d , Couleurs : %d)\n",clock()/CLOCKS_PER_SEC,T,check_solution(),couleur_max());
            clock_last = clock();
        }
        T = T*a;
    }
    //printf("T = %2.4f : (Conflits : %d , Couleurs : %d)\n",T,check_solution(),couleur_max());
}

int main(int argc, char* args[]) {
    if(argc>=2){
        if(lireDonnee(args[1])){
            init_arrays();
            init_adj();
            int indice;
            for(int i = 0;i<nb_sommets;i++){
                indice = choix();
                colorier(indice);
                update_node(indice);
            }
            int result = check_solution();
            printf("DSATUR : Conflits : %d , Couleurs : %d\n",result,couleur_max());
            //recuit(1000);
            genetic(1000,890,1000000);
            result = check_solution();
            printf("RECUIT : Conflits : %d , Couleurs : %d\n",result,couleur_max());
            
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
