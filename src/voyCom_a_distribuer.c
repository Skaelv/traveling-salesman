#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>

#define MAXMOT 256
#define MAXS 500

/**
 * Structure pour representer un cycle
 */
typedef struct t_cycle 
{
  int taille;   //la taille du cycle en construction (Nb ville)
  double poids; //le co�t du cycle (distance parcourue)
  int c[MAXS];  //liste des "taille" sommets
} t_cycle;

// t_cycle* copie_cycle( const t_cycle src){
//	this.taille = src.taille;
//	this.poids = src.poids;
//	for(int i = 0 ; i < src.taille ; i++)
//	{
//		this.c[i]=src.c[i];
//	}
//	return *this;
//}

/**
 * Charge le CSV des coordonn�es des villes.
 * 
 * @param [in] f le fichier
 * @param [out] nb_villes le nombre de villes de l'instance
 * @param [out] dist le tableau des nb_villes*nb_villes distances
 * @param [out] absc le tableau des abscisses des villes
 * @param [out] ord le tableau des ordonn�es des villes
 */
void lire_donnees(const char *f, unsigned int *nb_villes, double ***dist,  double **absc, double **ord)
{
  //double *absc; ///tableau des ordonn�es
  //double *ord;  /// tableau des abscisses
  char ligne[MAXMOT];
  FILE * fin = fopen(f,"r");

  if(fin != NULL)
    {
      //On recupere le nombre de villes
      fgets(ligne, MAXMOT, fin);
      *nb_villes = atoi(ligne);
      (*dist) = (double**)malloc(*nb_villes * sizeof(double*));
      (*absc) = (double*)malloc(*nb_villes * sizeof(double));
      (*ord) = (double*)malloc(*nb_villes * sizeof(double));
      int i = 0;
      while (fgets(ligne, MAXMOT, fin) != NULL) 
	{      
	  char *p = strchr(ligne, ';');
	  ligne[strlen(ligne) - strlen(p)]='\0';
	  p = &p[1];
	  (*absc)[i] = atof(ligne);
	  (*ord)[i] = atof(p);
	  i = i + 1;
	}
    }
  else
    {
      printf("Erreur de lecture du fichier.\n");
      exit(2);
    }
  fclose(fin);
  int i,j;

  //Calclul des distances
  for(i = 0; i < *nb_villes; i++)
    {
      (*dist)[i] = (double*) malloc(*nb_villes * sizeof(double));
      for(j = 0; j < *nb_villes; j++)
		{
		  (*dist)[i][j] = sqrt( ((*absc)[i] - (*absc)[j])* ((*absc)[i] - (*absc)[j]) + ((*ord)[i] - (*ord)[j]) * ((*ord)[i] - (*ord)[j]) );
		}
    }
}


/**
 * Supprime la structure des distances
 *
 * @param [in] nb_villes le nombre de villes.
 * @param [in,out] distances le tableau � supprimer.
 * @param [in,out] abscisses un autre tableau � supprimer.
 * @param [in,out] ordonnees encore un autre tableau � supprimer.
 */
void supprimer_distances_et_coordonnees(const int nb_villes, double **distances, double *abscisses, double *ordonnees)
{
  int i;
  for(i = 0; i < nb_villes; i++)
    {
      free(distances[i]);
    }
 free(distances);
 free(abscisses);
 free(ordonnees);
}


/**
 * Export le cycle dans un fichier HTML pour pouvoir �tre visualis�
 * dans l'applet.
 *
 * @param [in] cycle le cycle � afficher
 */
void afficher_cycle_html(const t_cycle cycle, double *posX, double *posY)
{
  FILE * fout = fopen("src/DisplayTsp.html","w");


  if(fout != NULL)
    {
      int i;
      fprintf(fout, "<html>\n <applet codebase=\".\" code=\"DisplayTsp.class\" width=600 height=600>\n");
      fprintf(fout, "<param name = Problem value = \"custom\">\n");
      fprintf(fout, "<param name = Problem CitiesPosX value = \"");
      for(i = 0; i < cycle.taille; i++)
	fprintf(fout,"%f;",posX[i]);
      fprintf(fout, "\">\n");
      fprintf(fout, "<param name = Problem CitiesPosY value = \"");
      for(i = 0; i < cycle.taille; i++)
	fprintf(fout,"%f;",posY[i]);
      fprintf(fout, "\">\n");
      fprintf(fout, "<param name = Parcours value = \"");
      fprintf(fout,"%d",cycle.c[0]);
      for(i = 1; i < cycle.taille; i++)
	fprintf(fout,"-%d",cycle.c[i]);
      fprintf(fout,"\">\n</applet>\n </html>\n");
    }
  else
  {
	  printf("Erreur d'écriture dans le fichier");
  }
  fclose(fout);
}

/**
 * Affiche le tableau des distances.
 *
 * @param [in] nb le nombre de villes
 * @param [in] distances le tableau
 */
void afficher_distances(const int nb, double **distances)
{
  unsigned int i ;
  unsigned int j ;
  for(i = 0  ; i < nb; i++)
    {   
      for(j = 0 ; j < nb ; j++)
	printf("%f ", distances[i][j]);
      printf("\n");
    }
  printf("\n");
}


/**
 * Fonction de comparaison pour le trie des ar�tes par leur poids.
 *
 * @param [in] v1 pointeur vers un triplet (i,j,poids)
 * @param [in] v2 pointeur verts un triplet (i,j,poids)
 * @return vrai si poid v1 < poids v2
 */
int comparer(const void *v1, const void *v2)
{
  double **px1 = (double **) v1;
  double **px2 = (double **) v2;

  double *x1 = *px1;
  double *x2 = *px2;
  if(x1[2] - x2[2] < 0)
    return -1;
  else 
    {
      if(x1[2] - x2[2] == 0)
	return 0;
      else
	return 1;
    }
}

/**
 * Construit un tableau de n*(n-1)/2 ar�tes tri� selon leur poids.
 *
 * @note utile pour le Kruskal
 *
 * @param [in] n le nombre de villes
 * @param [in] d tableau des n x n distances.
 * @return tableau d'ar�tes tri�es T[i][j] = poids_ij 
 */
double **trier_aretes(const int n, double **d)
{
  assert(d);
  
  int nb_aretes = n * (n - 1) / 2;
  double **T = (double **)malloc(nb_aretes * sizeof(double *));
  int i, j;
  int a = 0;

  //On initialise la structure d'ar�tes
  for(i = 0; i < n-1; i++)
    {
      for(j = i+1; j < n; j++)
	{
	  T[a] = (double *)malloc(3 * sizeof(double));
	  T[a][0] = i;
	  T[a][1] = j;
	  T[a][2] = d[i][j];
	  a++;
	}
    }
  
  //Appel au quicksort avec la bonne fonction de comparaison
  qsort(T, a, sizeof(T[0]), comparer);
  

  //Decommenter pour v�rifier le tri
  /*
  for(i = 0; i < a; i++)
    printf("%f ", T[i][2]);
  printf("\n");
  */
  return T;
}


/**
 * Supprime le tableau des ar�tes.
 *
 * @param [in] nb_villes le nombre de villes
 * @param [in,out] T le tableau � supprimer
 */
void supprimer_aretes(const int nb_villes, double **T)
{
  assert(T);

  int nb_aretes = nb_villes*(nb_villes - 1 ) / 2;
  unsigned int i;
  for( i = 0; i < nb_aretes ; ++i)
    free( T[i] );

  free(T);
}

/**
 * Calcul de l'algo naif du traveling-salesman
 *
 */
void pvc_exact_naif (const int nbVilles , double ** dist ,t_cycle * chemin , t_cycle * meilleur)
{

	//On test si un chemin est complet
	if (chemin->taille == nbVilles - 1 ) // n-1 car taille compte les arrêtes : jonctions entre villes
	{

		//Comparaison entre chemin et meilleur
		if( (chemin->poids <= meilleur->poids) || (meilleur->taille == 0 ) ) //meilleur->taille = 0 -->Initialisation
		{
			//Copie de chemin dans meilleur ->Copie de valeur, surtout pas d'adresse
			meilleur->taille = chemin->taille;
			meilleur->poids = chemin->poids;
			int i;
			for (i=0;i<=nbVilles;i++)
			{
				meilleur->c[i]=chemin->c[i];
			}
		}
	}
	else
	{
		//Si le chemin n'est pas complet on parcours toutes les villes pour calculer des nouveaux chemins
		int iterVilles;
		for(iterVilles = 0 ; iterVilles<nbVilles ; iterVilles++)
		{
			int villesParcourues;
			int bool = 1;
			//Si la ville est déjà dans le chemin, bool devient false et on ne traite pas l'étape suivante
			for ( villesParcourues = 0;villesParcourues <= chemin->taille; villesParcourues++)
			{
				if (chemin->c[villesParcourues] == iterVilles )
				{
					bool = 0;
				}
			}
			if (iterVilles==0)	//Test pour le premier passage, taille = 0 ne rentre pas dans la boucle alors qu'on le devrait
				bool=0;
			//Etape de construction des chemins
			if (bool)
			{
				//Calcul du poids entre la dernière ville parcourue et la ville actuelle
				chemin->poids += dist[chemin->c[chemin->taille]][iterVilles];
				chemin->taille++;
				//On place la nouvelle ville dans le tableau c[] dans une nouvelle case
				chemin->c[chemin->taille]=iterVilles;
				//Appel recursif pour terminer le chemin en cours
				pvc_exact_naif( nbVilles, dist, chemin, meilleur);
				//Suppression des modifs pour pouvoir redonner un chemin "clean" dans la suite de la boucle courante
				chemin->c[chemin->taille]=0;
				chemin->taille--;
				chemin->poids -= dist[chemin->c[chemin->taille]][iterVilles];
			}
		}
	}
}


void pvc_exact_branch_and_bound (const int nbVilles , double ** dist ,t_cycle * chemin , t_cycle * meilleur)
{
	if (meilleur->poids > chemin->poids) //Qu2 : Enleve les tests inutiles
	{
		//On test si un chemin est complet
		if (chemin->taille == nbVilles  ) // n-1 car taille compte les arrêtes : jonctions entre villes
		{

			//Comparaison entre chemin et meilleur
			if( (chemin->poids <= meilleur->poids) || (meilleur->taille == 0 ) ) //meilleur->taille = 0 -->Initialisation
			{
				//Copie de chemin dans meilleur ->Copie de valeur, surtout pas d'adresse
				meilleur->taille = chemin->taille;
				meilleur->poids = chemin->poids;
				int i;
				for (i=0;i<=nbVilles;i++)
				{
					meilleur->c[i]=chemin->c[i];
				}
			}
		}
		else
		{
			//Si le chemin n'est pas complet on parcours toutes les villes pour calculer des nouveaux chemins
			int iterVilles;
			for(iterVilles = 0 ; iterVilles<nbVilles ; iterVilles++)
			{
				int villesParcourues;
				int bool = 1;
				//Si la ville est déjà dans le chemin, bool devient false et on ne traite pas l'étape suivante
				for ( villesParcourues = 0;villesParcourues <= chemin->taille; villesParcourues++)
				{
					if (chemin->c[villesParcourues] == iterVilles )
					{
						bool = 0;
					}
				}
				if (iterVilles==0)	//Test pour le premier passage, taille = 0 ne rentre pas dans la boucle alors qu'on le devrait
					bool=0;
				//Etape de construction des chemins
				if (bool)
				{
					//Calcul du poids entre la dernière ville parcourue et la ville actuelle
					chemin->poids += dist[chemin->c[chemin->taille]][iterVilles];
					chemin->taille++;
					//On place la nouvelle ville dans le tableau c[] dans une nouvelle case
					chemin->c[chemin->taille]=iterVilles;
					//Appel recursif pour terminer le chemin en cours
					pvc_exact_branch_and_bound( nbVilles, dist, chemin, meilleur);
					//Suppression des modifs pour pouvoir redonner un chemin "clean" dans la suite de la boucle courante
					chemin->c[chemin->taille]=0;
					chemin->taille--;
					chemin->poids -= dist[chemin->c[chemin->taille]][iterVilles];
				}
			}
		}

	}

}


void pvc_approche_ppv(const int nbVilles , double ** dist , t_cycle * meilleur, t_cycle * chemin)
{

	//On test si un chemin est complet
	if (chemin->taille == nbVilles - 1 ) // n-1 car taille compte les arrêtes : jonctions entre villes
	{

		chemin->c[nbVilles]=chemin->c[0];
		chemin->poids += dist[chemin->c[chemin->taille]][chemin->c[0]];
		chemin->taille++;
		//Copie de chemin dans meilleur ->Copie de valeur, surtout pas d'adresse
		meilleur->taille = chemin->taille;
		meilleur->poids = chemin->poids;
		int i;
		for (i=0;i<=nbVilles;i++)
		{
			meilleur->c[i]=chemin->c[i];
		}
	}
	else
	{
		static int tmp = 2;
		int iterVilles;
		if (tmp <9)tmp++;else(tmp--);//modifie la valeur de tmp pour éviter le test de distance entre une ville et elle meme

		for(iterVilles = 0 ; iterVilles<nbVilles ; iterVilles++)
		{
				int villesParcourues;
				int bool = 1;

				//Si la ville est déjà dans chemin, bool devient false et on ne traite pas l'étape suivante
				for ( villesParcourues = 0;villesParcourues <= chemin->taille; villesParcourues++)
				{
					if (chemin->c[villesParcourues] == iterVilles )
					{
						bool = 0;
					}
				}


				//Etape de construction des chemins
				if (bool)
				{
					if ( (iterVilles != chemin->c[chemin->taille]) && (dist[chemin->c[chemin->taille]][tmp] > dist[chemin->c[chemin->taille]][iterVilles]) )
					{
						tmp = iterVilles;
					}
				}
		}

		//printf("temp : %d \n",tmp);
		//Ajout du poids entre la dernière ville parcourue et la ville actuelle
		chemin->poids += dist[chemin->c[chemin->taille]][tmp];
		chemin->taille++;
		//On place la nouvelle ville dans le tableau c[] dans une nouvelle case
		chemin->c[chemin->taille]=tmp;
		//Appel recursif pour terminer le chemin en cours
			pvc_approche_ppv( nbVilles, dist, chemin, meilleur);

	}
}
/**
 * Fonction main.
 */
int main (int argc, char *argv[])
{
  double **distances;
  double *abscisses;
  double *ordonnees;
  unsigned int nb_villes;

  //Exemple de mesure du temps
  lire_donnees("src/defi250.csv", &nb_villes, &distances, &abscisses, &ordonnees);
  t_cycle chemin;
  t_cycle meilleur;
  nb_villes = 10;
  	  //---------------------------Init des structs
  chemin.taille = 0;// ville 0;
  chemin.poids = 0;
  chemin.c[0] = 0;//on démarre de la ville 0
  meilleur.taille = 0;
  meilleur.poids=10000;
  meilleur.c[0]=0;



  //Initialisation du timer pour mesurer des temps (compiler avec -lrt) 
  struct timespec myTimerStart;
  clock_gettime(CLOCK_REALTIME, &myTimerStart);

  //pvc_approche_ppv(nb_villes,distances,&chemin,&meilleur);
  pvc_exact_branch_and_bound(nb_villes,distances,&chemin,&meilleur);
//  pvc_exact_naif(nb_villes,distances,&chemin,&meilleur);

  //R�cup�ration du timer et affichage
  struct timespec current;
  clock_gettime(CLOCK_REALTIME, &current); //Linux gettime
  double elapsed_in_ms =    (( current.tv_sec - myTimerStart.tv_sec) *1000 +
          ( current.tv_nsec - myTimerStart.tv_nsec)/1000000.0);
  printf("Temps passé (ms) : %lf\n", elapsed_in_ms);

  // Affichage des distances
  //afficher_distances(nb_villes,distances);


  afficher_cycle_html(meilleur, abscisses, ordonnees);
  
  double ** Aretes =  trier_aretes(nb_villes, distances);
  /// <-- Kruskal Here
  supprimer_aretes(nb_villes, Aretes);

  supprimer_distances_et_coordonnees(nb_villes, distances, abscisses, ordonnees);
  return 0;
}
