#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#ifndef F_PI
#define F_PI		(float)M_PI
#endif

#ifndef DEBUG
#define DEBUG		false
#endif

#ifndef NUMT
#define NUMT		2
#endif

#ifndef NUMTRIALS
#define NUMTRIALS	50000
#endif

#ifndef NUMTRIES
#define NUMTRIES	30
#endif

const float GMIN =	10.0;
const float GMAX =	20.0;
const float HMIN =	20.0;
const float HMAX =	30.0;
const float DMIN  =	10.0;
const float DMAX  =	20.0;
const float VMIN  =	20.0;
const float VMAX  =	30.0;
const float THMIN = 	70.0;
const float THMAX =	80.0;

const float GRAVITY =	-9.8f;
const float TOL = 5.0f;

float		Ranf( float, float );
int		Ranf( int, int );
void		TimeOfDaySeed( );

inline
float Radians( float degrees )
{
	return (F_PI/180.f) * degrees;
}

int
main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
	fprintf( stderr, "No OpenMP support!\n" );
	return 1;
#endif

	TimeOfDaySeed( );

	omp_set_num_threads( NUMT );

	float *vs  = new float [NUMTRIALS];
	float *ths = new float [NUMTRIALS];
	float * gs = new float [NUMTRIALS];
	float * hs = new float [NUMTRIALS];
	float * ds = new float [NUMTRIALS];

	for( int n = 0; n < NUMTRIALS; n++ )
	{
		vs[n]  = Ranf(  VMIN,  VMAX );
		ths[n] = Ranf( THMIN, THMAX );
		gs[n]  = Ranf(  GMIN,  GMAX );
		hs[n]  = Ranf(  HMIN,  HMAX );
		ds[n]  = Ranf(  DMIN,  DMAX );
	}

	double maxPerformance = 0.;
	int numHits;

	for( int tries = 0; tries < NUMTRIES; tries++ )
	{
		double time0 = omp_get_wtime( );

		numHits = 0;

		#pragma omp parallel for default(shared) reduction(+:numHits)
		for( int n = 0; n < NUMTRIALS; n++ )
		{
			float v   = vs[n];
			float thr = Radians( ths[n] );
			float vx  = v * cos(thr);
			float vy  = v * sin(thr);
			float  g  =  gs[n];
			float  h  =  hs[n];
			float  d  =  ds[n];

			// time for ball to return to y=0: t = -vy / (0.5*GRAVITY)
			float t = -vy / ( 0.5f * GRAVITY );
			float x = vx * t;

			if( x <= g )
			{
				if( DEBUG ) fprintf( stderr, "Ball doesn't even reach the cliff\n" );
			}
			else
			{
				// time ball reaches the cliff face (x = g)
				t = g / vx;
				float y = vy*t + 0.5f*GRAVITY*t*t;

				if( y <= h )
				{
					if( DEBUG ) fprintf( stderr, "Ball hits the cliff face\n" );
				}
				else
				{
					// quadratic: 0.5*GRAVITY*t^2 + vy*t - h = 0
					float A = 0.5f * GRAVITY;
					float B = vy;
					float C = -h;

					float disc = B*B - 4.f*A*C;

					if( disc < 0.f )
					{
						if( DEBUG ) fprintf( stderr, "Ball doesn't reach the upper deck.\n" );
						// shouldn't happen given the ranges, but skip gracefully
					}
					else
					{
						float sqrtdisc = sqrtf( disc );
						float t1 = (-B + sqrtdisc) / (2.f*A);
						float t2 = (-B - sqrtdisc) / (2.f*A);

						float tmax = (t2 > t1) ? t2 : t1;

						// horizontal distance from cliff edge on the upper deck
						float upperDist = vx * tmax - g;

						if( fabs( upperDist - d ) <= TOL )
						{
							if( DEBUG ) fprintf( stderr, "Hits the castle at upperDist = %8.3f\n", upperDist );
							numHits++;
						}
						else
						{
							if( DEBUG ) fprintf( stderr, "Misses the castle at upperDist = %8.3f\n", upperDist );
						}
					}
				}
			}
		}

		double time1 = omp_get_wtime( );
		double megaTrialsPerSecond = (double)NUMTRIALS / ( time1 - time0 ) / 1000000.;
		if( megaTrialsPerSecond > maxPerformance )
			maxPerformance = megaTrialsPerSecond;
	}

	float probability = (float)numHits / (float)(NUMTRIALS);

// #define CSV
#ifdef CSV
	fprintf(stderr, "%2d , %8d , %6.2lf\n", NUMT, NUMTRIALS, maxPerformance);
#else
	fprintf(stderr, "%2d threads : %8d trials ; probability = %6.2f%% ; megatrials/sec = %6.2lf\n",
		NUMT, NUMTRIALS, 100.*probability, maxPerformance);
#endif

	delete [] vs;
	delete [] ths;
	delete [] gs;
	delete [] hs;
	delete [] ds;

	return 0;
}

float
Ranf( float low, float high )
{
	float r = (float) rand();
	float t = r / (float) RAND_MAX;
	return low + t * ( high - low );
}

int
Ranf( int ilow, int ihigh )
{
	float low  = (float)ilow;
	float high = ceil( (float)ihigh );
	return (int) Ranf(low,high);
}

void
TimeOfDaySeed( )
{
	time_t now;
	time( &now );

	struct tm n;
	struct tm jan01;
#ifdef WIN32
	localtime_s( &now, &n );
	localtime_s( &now, &jan01 );
#else
	n     = *localtime(&now);
	jan01 = *localtime(&now);
#endif
	jan01.tm_mon  = 0;
	jan01.tm_mday = 1;
	jan01.tm_hour = 0;
	jan01.tm_min  = 0;
	jan01.tm_sec  = 0;

	double seconds = difftime( now, mktime(&jan01) );
	unsigned int seed = (unsigned int)( 1000.*seconds );
	srand( seed );
}
