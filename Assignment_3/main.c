#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define RESET "\x1B[0m"
#define GRN   "\x1B[32m"

#define BUFFER_SIZE 512

//structure to store the vehicle details

struct vehicleinfo
{  
    char licence[9];
	int group;
	int elapsed_time;
	int crossing_time;
}vehicles[20];


//shared
static int no_vehicles;
static int max_carEquivalent;

// Mutex and condition var
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// mutual exclusion variable
static int curload = 0;

//initialising thread function
void *vehicleinfo(void *arg);

//Main function

int main(int argc, char *argv[])
{
pthread_t child[20];
int nchildren = 0;
int i = 0;
char buffer[BUFFER_SIZE];
	
// I/O redirection of file into a struncture
if(argc == 1)
{
while(fgets(buffer, BUFFER_SIZE, stdin))
{
if(i == 0){
sscanf(buffer,"%d",&max_carEquivalent);
}
else{
sscanf(buffer, "%s\t%d\t%d\t%d\n",vehicles[no_vehicles].licence, &vehicles[no_vehicles].group, &vehicles[no_vehicles].elapsed_time, &vehicles[no_vehicles].crossing_time);
no_vehicles += 1;}
i++;}
}
else{
printf(" Usage : ./a.out < input_file.txt \n ");
exit(0);
}

//setting the output color to green     	 
printf(GRN "\n" );
printf( "Maximum car equivalent of the  tunnel is %d \n" ,max_carEquivalent );
printf("--------------------------------------------------- \n");

// total no.of vehicles given by the user 
printf("Total no of vehicles given by the user %d \n",no_vehicles);
printf("\n");

printf("**************************************************************************************\n");
printf("LICENSE           GROUP          CAR_EQV               ACTION         CURRENT LOAD \n");
printf("***************************************************************************************\n");

// start the thread 
for(i = 0; i < no_vehicles; i++)
{
//sleep for elapsed time until the vehicle arrives
sleep(vehicles[i].elapsed_time);
pthread_create(&child[nchildren++], NULL, vehicleinfo, (void *)&vehicles[i]); 
}
// Joining terminated threads
for(i = 0; i < no_vehicles; i++)
{
pthread_join(child[i], NULL);
}
exit(0);
}

// tunnel admission rules function
void entertunnel(struct vehicleinfo **vehicles,int vehicle_load)
{
if((*vehicles)->group==3){
printf("%9.9s           %d               %d                  entered            %d\n",(*vehicles)->licence,(*vehicles)->group,vehicle_load,curload);
//sleep for crossing time
sleep((*vehicles)->crossing_time); 
}
else{

//Increasing the load of the tunnel
curload=curload+vehicle_load;
printf("%9.9s           %d               %d                  entered            %d\n",(*vehicles)->licence,(*vehicles)->group,vehicle_load,curload);
//unlock th mutex
pthread_mutex_unlock(&lock);

//sleep for crossing time
sleep((*vehicles)->crossing_time);
}
}

//Tunnel exit rules function
void leavetunnel(struct vehicleinfo **vehicles,int vehicle_load)
{
if((*vehicles)->group==3){
printf("%9.9s           %d               %d                  exited             %d\n",(*vehicles)->licence,(*vehicles)->group,vehicle_load,curload);
}
else{

pthread_mutex_lock(&lock);
//Decreasing the tunnel load
curload=curload-vehicle_load;
//signal the vehicle has left
pthread_cond_signal(&cond);
pthread_mutex_unlock(&lock);
printf("%9.9s           %d               %d                  exited             %d\n",(*vehicles)->licence,(*vehicles)->group,vehicle_load,curload);
}
}

//rejection method when max_careq<5
void rejectvehicle(struct vehicleinfo **vehicles,int vehicle_load)
{
printf("%9.9s           %d               %d                  reject             %d\n",(*vehicles)->licence,(*vehicles)->group,vehicle_load,curload);
}

//Thread function

void *vehicleinfo(void *arg)
{
	
	/*
		Method that takes in a structure of 'vehicleinfo' and simulates processing with a vehicle.
		Locks mutex when vehicleinfo enters and goes to a vehicle/waits for one.
		Unlocks mutex when vehicleinfo is being taken care of.
		Locks mutex when vehicleinfo is leaving and vehicle can be free (also signals condition var).
	
		Returns nothing
	*/
struct vehicleinfo *vehicles;
vehicles = (struct vehicleinfo *)arg;

//initialising a mutex
pthread_mutex_init(&lock, NULL);
int careq;
// assigning careq to vehicles
if(vehicles->group==1){
careq=1;
}
else if (vehicles->group==2)
{
careq=5;
}
else if(vehicles->group==3){
careq=0;
}

printf("%9.9s           %d               %d                  arrived            %d\n",vehicles->licence, vehicles->group,careq,curload);


 
//condition to check max-careq and group number
if((max_carEquivalent<5) && (vehicles->group==2))
{
rejectvehicle(&vehicles,careq);
}
else{

if(vehicles->group==1||vehicles->group==2)
{
//  Acquring lock for Critical section
pthread_mutex_lock(&lock);

//CRITICAL SECTION
while(curload+careq>max_carEquivalent) 
{
pthread_cond_wait(&cond, &lock);// No vehicles available, so wait
}
}
// if the vehicle is accepted by the tunnel do the procesiing else reject
// vehicleinfo can be processed now
entertunnel(&vehicles,careq);
leavetunnel(&vehicles,careq);
// Signal that the vehicleinfo has been taken care of
}
}
// }


