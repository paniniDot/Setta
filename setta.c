#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>

#include "DBGpthread.h"
#include "printerror.h"

#define SACERDOTI_TOT 16
#define SACERDOTI_NOVIZI 10
#define SACERDOTI_ANZIANI 6

int nInginocchiamentiTutti = 0; /* nr. di volte in cui tutti si sono inginocchiati */
int nPersoneInginocchiate = 0; /* persone attualmente inginocchiate */

pthread_mutex_t mutexDistributoreBuono;
pthread_mutex_t mutexDistributoreSanto;
pthread_mutex_t mutexInginocchiamento;
pthread_cond_t condInginocchio;

void *anziano( void *arg ) {
    char Alabel[128];
    int nInginocchiamentiPersonali = 0;
    sprintf( Alabel, "Sacerdote anziano %" PRIiPTR "", ( intptr_t )arg );
    while( 1 ) {

        DBGpthread_mutex_lock( &mutexDistributoreBuono, Alabel );
        printf( "%s: prende VINO BUONO e si sbronza\n", Alabel );
        DBGpthread_mutex_unlock( &mutexDistributoreBuono, Alabel );

        DBGpthread_mutex_lock( &mutexDistributoreSanto, Alabel );
        printf( "%s: prende VINO SANTO\n", Alabel );
        DBGpthread_mutex_unlock( &mutexDistributoreSanto, Alabel );

        DBGpthread_mutex_lock( &mutexInginocchiamento, Alabel );
        while( nInginocchiamentiPersonali != nInginocchiamentiTutti ) {
            DBGpthread_cond_wait( &condInginocchio, &mutexInginocchiamento, Alabel );
        }
        sleep( 1 );
        printf( "%s: si inginocchia per la %d volta (totali = %d) e versa il VINO SANTO\n", Alabel, nInginocchiamentiPersonali, nInginocchiamentiTutti );
        nInginocchiamentiPersonali++;
        nPersoneInginocchiate++;
        if( nPersoneInginocchiate == SACERDOTI_TOT ) {
            printf( "%s: Sono l\'ultimo\n", Alabel );
            nInginocchiamentiTutti++;
            nPersoneInginocchiate = 0;
        }
        printf( "%s: ha finito di inginocchiarsi\n", Alabel );
        DBGpthread_cond_broadcast( &condInginocchio, Alabel );
        DBGpthread_mutex_unlock( &mutexInginocchiamento, Alabel );
    }
    pthread_exit( NULL );
}

void *novizio( void *arg ) {
    char Nlabel[128];
    int nInginocchiamentiPersonali = 0;
    sprintf( Nlabel, "Sacerdote novizio %" PRIiPTR "", ( intptr_t )arg );
    while( 1 ) {

        DBGpthread_mutex_lock( &mutexDistributoreSanto, Nlabel );
        printf( "%s: prende VINO SANTO\n", Nlabel );
        DBGpthread_mutex_unlock( &mutexDistributoreSanto, Nlabel );

        DBGpthread_mutex_lock( &mutexInginocchiamento, Nlabel );
        while( nInginocchiamentiPersonali != nInginocchiamentiTutti ) {
            DBGpthread_cond_wait( &condInginocchio, &mutexInginocchiamento, Nlabel );
        }
        sleep( 1 );
        printf( "%s: si inginocchia per la %d volta (totali = %d) e versa il VINO SANTO\n", Nlabel, nInginocchiamentiPersonali, nInginocchiamentiTutti );
        nInginocchiamentiPersonali++;
        nPersoneInginocchiate++;
        if( nPersoneInginocchiate == SACERDOTI_TOT ) {
             printf( "%s: Sono l\'ultimo\n", Nlabel );
            nInginocchiamentiTutti++;
            nPersoneInginocchiate = 0;
        }
        printf( "%s: ha finito di inginocchiarsi\n", Nlabel );
        DBGpthread_cond_broadcast( &condInginocchio, Nlabel );
        DBGpthread_mutex_unlock( &mutexInginocchiamento, Nlabel );

    }
    pthread_exit( NULL );
}

int main( void ) {
    int rc;
    pthread_t th;
    intptr_t i;

    DBGpthread_mutex_init( &mutexDistributoreBuono, NULL, "main" );
    DBGpthread_mutex_init( &mutexDistributoreSanto, NULL, "main" );
    DBGpthread_mutex_init( &mutexInginocchiamento, NULL, "main" );
    DBGpthread_cond_init( &condInginocchio, NULL, "main" );

    for( i=0; i<SACERDOTI_ANZIANI; i++ ) {
        rc = pthread_create( &th, NULL, anziano, ( void *)i );
        if( rc ) {
            PrintERROR_andExit( rc, "Creazione anziano" );
        }
    }

    for( i=0; i<SACERDOTI_NOVIZI; i++ ) {
        rc = pthread_create( &th, NULL, novizio, ( void *)i );
        if( rc ) {
            PrintERROR_andExit( rc, "Creazione anziano" );
        }
    }

    pthread_exit( NULL );
    return 0;
}

