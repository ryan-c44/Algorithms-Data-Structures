#include <iostream>
#include <fstream>
#include <cstring>


struct Event {
    double time1; // time1 arrival time or service finish time 
    double time2;
    int type; // type is tourist or business passegner or tourist/business server 
};

void push_business(Event &server, Event (&business_servers)[20]);
void push_tourist(Event &server,  Event (&tourist_servers)[20]);
void siftdown(Event&, int);
bool isEmpty();
void arrival_event(Event (&event_heap)[41], Event (&tourist_servers)[20], Event (&business_servers)[20],  Event (&tourist_queue)[500],  Event (&business_queue)[500], std::ifstream &inData);
void server_event(Event (&event_heap)[41], Event (&tourist_servers)[20], Event (&business_servers)[20],  Event (&tourist_queue)[500],  Event (&business_queue)[500]);
void pop_tourist(Event &server, Event (&tourist_servers)[20]);
void pop_business(Event &server, Event (&business_servers)[20]);
void siftup(Event (&event_heap)[41], int i);
void tourist_enqueue(Event (&tourist_queue)[500], Event &customer);
void tourist_dequeue(Event (&tourist_queue)[500], Event &customer);
void business_enqueue(Event (&business_queue)[500], Event &customer);
void business_dequeue(Event (&business_queue)[500], Event &customer);
void output();

double simulation_time = 0;
int event_counter = -1;

int touristFront = 0, touristRear = 0;

int businessFront = 0, businessRear = 0;

int tourist_server_count;
int idle_tourist = 0;

int business_server_count;
int idle_business = 0;

int passengers_served = 0;
double last_service_time = 0;

int max_tourist_queue = 0;
int tourist_queue_counter = 0;

int max_business_queue = 0;
int business_queue_counter = 0;



int main() {

    struct Event event_heap[41];
    struct Event tourist_queue[500];
    struct Event business_queue[500];
    struct Event tourist_servers[20];
    struct Event business_servers[20];

    std::ifstream inData;
    char fileName[10];

    // read in files
    std::cerr << "Enter a file name: ";
    std::cin >> fileName;

    // tests for file open
    inData.open(fileName);
    if(inData.fail()) {
        std::cerr << " File was unable to open " << std::endl;
        exit(EXIT_FAILURE);  
    }

    inData >> business_server_count >> tourist_server_count;

    for(int i = 0; i < business_server_count; i++) {
        Event server;
        push_business(server, business_servers);
    }

    for(int i = 0; i < tourist_server_count; i++) {
        Event server;
        push_tourist(server, tourist_servers);
    }

    Event customer;
    inData >> customer.time1 >> customer.type >> customer.time2;
    passengers_served++;

    event_heap[0] = customer;
    event_counter++;

    simulation_time = event_heap[0].time1;

    while(event_counter != -1) {
        
        simulation_time = event_heap[0].time1;
        last_service_time = event_heap[0].time1;

        std::cout << "Arrival/Service Time:  " << event_heap[0].time1 << std::endl;
        std::cout << "Service Time for customers: " << event_heap[0].time2 << std::endl;
        std::cout << "Event Type: " << event_heap[0].type << std::endl;
        
        if(event_heap[0].type == 0 || event_heap[0].type == 1) {

            arrival_event(event_heap, tourist_servers, business_servers, tourist_queue, business_queue, inData);

        } else {

            server_event(event_heap, tourist_servers, business_servers, tourist_queue, business_queue);
        }

    }

    output();

    inData.close();

    return 0;
}

void push_business(Event &server, Event (&business_servers)[20]) {
    // checks for stack overflow
    if(idle_business >= 20) {
        std::cerr << "Stack Overflow" << std::endl;
    } else {
        business_servers[idle_business] = server;
        idle_business = idle_business + 1;
    }
}

void push_tourist(Event &server,  Event (&tourist_servers)[20]) {
    // checks for stack overflow
    if(idle_tourist >= 20) {
        std::cerr<< "Stack Overflow" << std::endl;
    } else {
        tourist_servers[idle_tourist] = server;
        idle_tourist = idle_tourist + 1;
    }
}

void pop_tourist(Event &server, Event (&tourist_servers)[20]) {
    // checks for stack underflow 
    if(idle_tourist <= -1) {
        std::cerr << "Stack Underflow" << std::endl;
    } else {
        //removes top index from stack 
        server = tourist_servers[idle_tourist];
        idle_tourist = idle_tourist - 1; 
    }

}

void pop_business(Event &server, Event (&business_servers)[20]) {
    // checks for stack underflow 
    if(idle_business <= -1) {
        std::cerr << "Stack Underflow" << std::endl;
    } else {
        //removes top index from stack 
        server = business_servers[idle_business];
        idle_business = idle_business - 1; 
    }

}

void siftdown(Event (&event_heap)[41], int i) {
    
    // move element i down to its correct position 
    int c = i * 2 + 1;

    if(c > event_counter) {
        return;
    }

    if(c < event_counter - 1) {
        if(event_heap[c].time1 > event_heap[c + 1].time1) {
            c = c + 1;
        }
    }

    // tests values and swaps positon if true
    if(event_heap[i].time1 > event_heap[c].time1) {
        std::swap(event_heap[i], event_heap[c]);
        siftdown(event_heap, c);
    }

}


void siftup(Event (&event_heap)[41], int i) {
    
    // move element i down to its correct position 
    if(i == 0) {
        return;
    }

    int p = i/2;

    if(event_heap[p].time1 < event_heap[i].time1) {
        return;
    } else {
        std::swap(event_heap[i],event_heap[p]);
        siftup(event_heap,p);
    }    
}

void tourist_enqueue(Event (&tourist_queue)[500], Event &customer) {
    
    tourist_queue[touristRear] = customer;

    touristRear++;

    if(touristRear >= 500) {
        touristRear = 0;
    }
}

void tourist_dequeue(Event (&tourist_queue)[500], Event &customer) {

    if(touristFront == touristRear) {
        customer.type = -1;
    } else {
        customer = tourist_queue[touristFront];
        touristFront++;
    }

    if(touristFront >= 500) {

        touristFront = 0;
    }

}

void business_enqueue(Event (&business_queue)[500], Event &customer) {

    business_queue[businessRear] = customer;

    businessRear++;

    if(businessRear >= 500) {
        businessRear = 0;
    }
}

void business_dequeue(Event (&business_queue)[500], Event &customer) {


    if(businessFront == businessRear) {
        customer.type = -1;
    } else {
        customer = business_queue[businessFront];
        businessFront++;
    }

   if(businessRear >= 500) {
        businessRear = 0;
    }
    
}

void arrival_event(Event (&event_heap)[41], Event (&tourist_servers)[20], Event (&business_servers)[20],  Event (&tourist_queue)[500], Event (&business_queue)[500], std::ifstream &inData){
    
    Event current_arrival = event_heap[0];

    inData >> event_heap[0].time1 >> event_heap[0].type >> event_heap[0].time2;
    passengers_served++;

    if(event_heap[0].time1 == 0 && event_heap[0].type == 0 && event_heap[0].time2 == 0) {
        
        event_heap[0] = event_heap[event_counter];
        event_counter--;
        passengers_served--;

    }

    siftdown(event_heap, 0);

    if(current_arrival.type == 0) {
        if(idle_tourist > 0) {
            
            Event server;

            pop_tourist(server, tourist_servers);

            server.time1 = current_arrival.time2 + simulation_time;

            server.type = current_arrival.type + 2;

            event_counter++;

            event_heap[event_counter] = server;

            siftup(event_heap, event_counter);

        }else {

            tourist_enqueue(tourist_queue, current_arrival);

        }
    } else {
        if(idle_business > 0) {
          
            Event server;

            pop_business(server, business_servers);

            server.time1 = current_arrival.time2 + simulation_time;

            server.type = current_arrival.type + 2;

            event_counter++;

            event_heap[event_counter] = server;

            siftup(event_heap, event_counter);

        } else {

            business_enqueue(business_queue, current_arrival);

        }
    }
}

void server_event(Event (&event_heap)[41], Event (&tourist_servers)[20], Event (&business_servers)[20],  Event (&tourist_queue)[500],  Event (&business_queue)[500]) {

    if(event_heap[0].type == 2) {

        if(touristFront == touristRear) {

            push_tourist(event_heap[0], tourist_servers);

            event_heap[0] = event_heap[event_counter];
            event_counter--;

        } else {

            Event customer;

            tourist_dequeue(tourist_queue, customer);

            event_heap[0].time1 = customer.time2 + simulation_time;

        }

        siftdown(event_heap, 0);

    } else {

        if(businessFront == businessRear) {

            push_business(event_heap[0], business_servers);

            event_heap[0] = event_heap[event_counter];
            event_counter--;

        } else {

            Event customer;

            business_dequeue(business_queue, customer);

            event_heap[0].time1 = customer.time2 + simulation_time;

        }

        siftdown(event_heap, 0);

    }
}

void output() {

    std::cout << "Pass 1: Business servers exclusively serve business class" << std::endl;

    std::cout << " " << std::endl;

    std::cout << "Number of people served: " << passengers_served << std::endl;

    std::cout << "Time of last service: " << last_service_time << std::endl;

    std::cout << " " << std::endl;

    std::cout << "Business class customers:" << std::endl;
    std::cout << "Average total service time: " << std::endl;
    std::cout << "Average total tme in queue: " << std::endl;
    std::cout << "Maximum number queued: " <<  business_queue_counter << std::endl;

    std::cout << " " << std::endl;

    std::cout << "Tourist class customers:" << std::endl;
    std::cout << "Average total service time: " << std::endl;
    std::cout << "Average total tme in queue: " << std::endl;
    std::cout << "Maximum number queued: " << tourist_queue_counter << std::endl;


}


