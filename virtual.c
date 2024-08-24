#include <stdio.h>
#include <limits.h>
#include "oslabs.h"


int process_page_access_fifo(struct PTE page_table[TABLEMAX],
int *table_cnt, 
int page_number, 
int frame_pool[POOLMAX],
int *frame_cnt,
int current_timestamp)
{
    /*First the function checks if the page being referenced is already in memory (i.e., the page-table
entry has the valid bit true). If so, it returns the frame number, after modifying the
last_access_timestamp and the reference_count fields of the page-table entry.*/

    if (page_table[page_number].is_valid !=0 ){
       page_table[page_number].reference_count += 1; //update reference count
       page_table[page_number].last_access_timestamp = current_timestamp; //update time
       return page_table[page_number].frame_number;
    }
    /*If the page being referenced is not in memory, the function checks if there are any free frames
(i.e., the process frame pool is not empty). If so, a frame is removed from the process frame
pool and the frame number is inserted into the page-table entry corresponding to the logical
page number. In addition, the other fields of the page-table entry are set appropriately. The
function returns the frame number associated with the page-table entry.
*/
    else if(page_table[page_number].is_valid == 0 && *frame_cnt > 0){
        *frame_cnt -= 1; //lowers frame count
        page_table[page_number].frame_number = frame_pool[*frame_cnt];//updates the number of free frames
        page_table[page_number].reference_count = 1; //update reference count
        page_table[page_number].last_access_timestamp = current_timestamp; //update time
        page_table[page_number].is_valid = 1; //moved to memory so it is valid
        page_table[page_number].arrival_timestamp = current_timestamp;//updates arrival time
        return page_table[page_number].frame_number;
    }

    /*If the page being referenced is not in memory and there are no free frames for the process, a
page needs to be replaced. The function selects among all the pages of the process that are
currently in memory (i.e., they have valid bits as true) the page that has the smallest
arrival_timestamp. It marks that page_table entry as invalid, along with setting the
frame_number, arrival_timestamp, last_access_timestamp and reference_count to -1. It then
sets the frame_number of the page-table entry of the newly-referenced page to the newly freed
frame. It also sets the arrival_timestamp, the last_access_timestamp and the reference_count
fields of the page-table entry appropriately. Finally, the function returns this frame number.
*/
    else {
        int earliest_access_timestamp = INT_MAX;
        int earliest_loc = 0;

        for(int i = 0; i < *table_cnt; i++){
            if(page_table[i].is_valid != 0){ //checks if the page is in memory 
                 if(page_table[i].arrival_timestamp < earliest_access_timestamp){ //checks if the arrival timestamp is lower than the last
                    earliest_access_timestamp = page_table[i].arrival_timestamp;//the arrival timestamp is updated 
                    earliest_loc = i;//the earliest arrival location is saved
                 }
            }
        }
        page_table[page_number].frame_number = page_table[earliest_loc].frame_number; //replaces the page in memory
        page_table[page_number].is_valid = 1; //sets it to valid so the fact its in memory is marked
        page_table[page_number].arrival_timestamp = current_timestamp;//updates arrival time
        page_table[page_number].last_access_timestamp = current_timestamp;//updates  last access time
        page_table[page_number].reference_count = 1;//this is the first time it was referenced the count is 1
        
        //Move to the page no longer in memory all values are set to null since it is no longer in memory

        page_table[earliest_loc].frame_number = -1;
        page_table[earliest_loc].is_valid = 0;
        page_table[earliest_loc].arrival_timestamp = -1;
        page_table[earliest_loc].last_access_timestamp = -1;
        page_table[earliest_loc].reference_count = -1;
        return page_table[page_number].frame_number;
    }    
    
}
int count_page_faults_fifo(struct PTE page_table[TABLEMAX],
int table_cnt,
int reference_string[REFERENCEMAX],
int reference_cnt,
int frame_pool[POOLMAX],
int frame_cnt)
{
    /*When simulating the processing of a page access, the function first checks if the page being
referenced is already in memory (i.e., the page-table entry has the valid bit true). If so, it
modifies the last_access_timestamp and the reference_count fields of the page-table entry. It
then moves on to the processing of the next entry in the sequence of logical page numbers.*/
    int page_fault_counter = 0;
    int timestamp = 1;
    for(int i = 0; i < reference_cnt; i++){
        if (page_table[reference_string[i]].is_valid !=0 ){
         page_table[reference_string[i]].reference_count += 1; //update reference count
         page_table[reference_string[i]].last_access_timestamp = timestamp; //update time
        }

     
   /*In order to simulate timestamps, the function starts with a timestamp of 1 and increments it
whenever the processing of a new page access is begun.

If the page being referenced is not in memory, the function checks if there are any free frames
(i.e., the process frame pool is not empty). If so, a frame is removed from the process frame
pool and the frame number is inserted into the page-table entry corresponding to the logical
page number. In addition, the other fields of the page-table entry are set appropriately. The
function counts this page access as a page fault.*/
        else if (page_table[reference_string[i]].is_valid == 0 && frame_cnt > 0){
            frame_cnt -= 1;
            page_table[reference_string[i]].frame_number = frame_pool[frame_cnt];//updates the number of free frames
            page_table[reference_string[i]].last_access_timestamp = timestamp; //update time
            page_table[reference_string[i]].arrival_timestamp = timestamp; //update time
            page_table[reference_string[i]].reference_count = 1; //update reference count
            page_table[reference_string[i]].is_valid = 1; //moved to memory so it is valid
            page_fault_counter++;
            timestamp++;
        }
    
/*If the page being referenced is not in memory and there are no free frames for the process, a
page needs to be replaced. The function selects among all the pages of the process that are
currently in memory (i.e., they have valid bits as true) the page that has the smallest
arrival_timestamp. It marks that page-table entry as invalid, along with setting the
arrival_timestamp, last_access_timestamp and reference_count to -1. It then sets the
frame_number of the page-table entry of the newly-referenced page to the newly freed frame. It
also sets the arrival_timestamp, the last_access_timestamp and the reference_count fields of
the page-table entry appropriately. The function counts this page access as a page fault.
The function returns the total number of page faults encountered in the simulation.*/
        else {
        int earliest_access_timestamp = INT_MAX;
        int earliest_loc = 0;

        for(int x = 0; x < table_cnt; x++){
            if(page_table[x].is_valid != 0){ //checks if the page is in memory 
                 if(page_table[x].arrival_timestamp < earliest_access_timestamp){ //checks if the arrival timestamp is lower than the last
                    earliest_access_timestamp = page_table[x].arrival_timestamp;//the arrival timestamp is updated 
                    earliest_loc = i;//the earliest arrival location is saved
                 }
            }
        }
        page_table[reference_string[i]].frame_number = page_table[earliest_loc].frame_number; //replaces the page in memory
        page_table[reference_string[i]].is_valid = 1; //sets it to valid so the fact its in memory is marked
        page_table[reference_string[i]].arrival_timestamp = timestamp;//updates arrival time
        page_table[reference_string[i]].last_access_timestamp = timestamp;//updates  last access time
        page_table[reference_string[i]].reference_count = 1;//this is the first time it was referenced the count is 1
        
        //Move to the page no longer in memory all values are set to null since it is no longer in memory

        page_table[earliest_loc].frame_number = -1;
        page_table[earliest_loc].is_valid = 0;
        page_table[earliest_loc].arrival_timestamp = -1;
        page_table[earliest_loc].last_access_timestamp = -1;
        page_table[earliest_loc].reference_count = -1;

        page_fault_counter++;
        timestamp++;
        
        }
    }
    return page_fault_counter;
}
int process_page_access_lfu(struct PTE page_table[TABLEMAX],
int *table_cnt, 
int page_number, 
int frame_pool[POOLMAX],
int *frame_cnt, 
int current_timestamp
)
{
    if (page_table[page_number].is_valid !=0 ){
       page_table[page_number].reference_count += 1; //update reference count
       page_table[page_number].last_access_timestamp = current_timestamp; //update time
       return page_table[page_number].frame_number;
    }
    /*If the page being referenced is not in memory, the function checks if there are any free frames
(i.e., the process frame pool is not empty). If so, a frame is removed from the process frame
pool and the frame number is inserted into the page-table entry corresponding to the logical
page number. In addition, the other fields of the page-table entry are set appropriately. The
function returns the frame number associated with the page-table entry.
*/
    else if(page_table[page_number].is_valid == 0 && *frame_cnt > 0){
        *frame_cnt -= 1; //lowers frame count
        page_table[page_number].frame_number = frame_pool[*frame_cnt];//updates the number of free frames
        page_table[page_number].reference_count = 1; //update reference count
        page_table[page_number].last_access_timestamp = current_timestamp; //update time
        page_table[page_number].is_valid = 1; //moved to memory so it is valid
        page_table[page_number].arrival_timestamp = current_timestamp;//updates arrival time
        return page_table[page_number].frame_number;
    }

    /*If the page being referenced is not in memory and there are no free frames for the process, a
page needs to be replaced. The function selects among all the pages of the process that are
currently in memory (i.e., they have valid bits as true) the page that has the smallest
arrival_timestamp. It marks that page_table entry as invalid, along with setting the
frame_number, arrival_timestamp, last_access_timestamp and reference_count to -1. It then
sets the frame_number of the page-table entry of the newly-referenced page to the newly freed
frame. It also sets the arrival_timestamp, the last_access_timestamp and the reference_count
fields of the page-table entry appropriately. Finally, the function returns this frame number.
*/
    else {
        int lowest_frq = INT_MAX;
        int lowest_loc = 0;

        for(int i = 0; i < *table_cnt; i++){

            if(page_table[i].is_valid != 0){ //checks if the page is in memory 
                 if (page_table[i].reference_count == page_table[lowest_loc].reference_count){ //if refrence counts are equal 
                    if(page_table[i].arrival_timestamp < page_table[lowest_loc].arrival_timestamp){ //compares arrival timestamp
                        lowest_loc = i;//moves lowest location to lower timestamp
                 }
                 else if (page_table[i].reference_count < page_table[lowest_loc].reference_count){ // compares the reference counts 
                    lowest_frq = page_table[i].reference_count; //saves the lowest frequency
                    lowest_loc = i; //saves the lowest location
                 }
            }
        }
        page_table[page_number].frame_number = page_table[lowest_loc].frame_number; //replaces the page in memory
        page_table[page_number].is_valid = 1; //sets it to valid so the fact its in memory is marked
        page_table[page_number].arrival_timestamp = current_timestamp;//updates arrival time
        page_table[page_number].last_access_timestamp = current_timestamp;//updates  last access time
        page_table[page_number].reference_count = 1;//this is the first time it was referenced the count is 1
        
        //Move to the page no longer in memory all values are set to null since it is no longer in memory

        page_table[lowest_loc].frame_number = -1;
        page_table[lowest_loc].is_valid = 0;
        page_table[lowest_loc].arrival_timestamp = -1;
        page_table[lowest_loc].last_access_timestamp = -1;
        page_table[lowest_loc].reference_count = -1;
        return page_table[page_number].frame_number;
    }
    }    
}
int count_page_faults_lfu(struct PTE page_table[TABLEMAX],
int table_cnt,
int refrence_string[REFERENCEMAX],
int reference_cnt,
int frame_pool[POOLMAX],
int frame_cnt)
{
    /*The function returns the estimated number of page faults for the reference string, with respect to
the pool of frames allocated to the process. For each logical page number (in the sequence),
the function simulates the processing of the page access in the LRU system. It keeps track of
the number of page faults that occur in the system as it simulates the processing of the entire
sequence of logical page numbers.*/
    int page_fault_counter = 0;
    int timestamp = 1;
/*When simulating the processing of a page access, the function first checks if the page being
referenced is already in memory (i.e., the page-table entry has the valid bit true). If so, it
modifies the last_access_timestamp and the reference_count fields of the page-table entry. It
then moves on to the processing of the next entry in the sequence of logical page numbers.
In order to simulate timestamps, the function starts with a timestamp of 1 and increments it
whenever the processing of a new page access is begun.*/
for(int i = 0; i < reference_cnt; i++){
    if (page_table[refrence_string[i]].is_valid !=0 ){
       page_table[refrence_string[i]].reference_count += 1; //update reference count
       page_table[refrence_string[i]].last_access_timestamp = timestamp; //update time
    }



/*If the page being referenced is not in memory, the function checks if there are any free frames
(i.e., the process frame pool is not empty). If so, a frame is removed from the process frame
pool and the frame number is inserted into the page-table entry corresponding to the logical
page number. In addition, the other fields of the page-table entry are set appropriately. The
function counts this page access as a page fault.*/
    else if(page_table[refrence_string[i]].is_valid == 0 && frame_cnt > 0){
        frame_cnt -= 1; //lowers frame count
        page_table[refrence_string[i]].frame_number = frame_pool[frame_cnt];//updates the number of free frames
        page_table[refrence_string[i]].reference_count = 1; //update reference count
        page_table[refrence_string[i]].last_access_timestamp = timestamp; //update time
        page_table[refrence_string[i]].is_valid = 1; //moved to memory so it is valid
        page_table[refrence_string[i]].arrival_timestamp = timestamp;//updates arrival time
        page_fault_counter += 1;
        
    }
/*If the page being referenced is not in memory and there are no free frames for the process, a
page needs to be replaced. The function selects among all the pages of the process that are
currently in memory (i.e., they have valid bits as true) the page that has the smallest
last_access_timestamp. It marks that page-table entry as invalid, along with setting the
arrival_timestamp, last_access_timestamp and reference_count to 0. It then sets the
frame_number of the page-table entry of the newly-referenced page to the newly freed frame. It
also sets the arrival_timestamp, the last_access_timestamp and the reference_count fields of
the page-table entry appropriately. The function counts this page access as a page fault.
The function returns the total number of page faults encountered in the simulation.*/
    else {
        int smallest_reference_count = INT_MAX;
        int smallest_loc = 0;

        for(int i = 0; i < table_cnt; i++){
            if(page_table[i].is_valid != 0){ //checks if the page is in memory 
                 if(page_table[i].reference_count < smallest_reference_count){ //checks if the references are lower than the last page
                    smallest_reference_count = page_table[i].reference_count;//The lowest reference count is saved for the loop 
                    smallest_loc = i;//the lowest referenced location is saved
                 }
            }
        }
        page_table[refrence_string[i]].frame_number = page_table[smallest_loc].frame_number; //replaces the page in memory
        page_table[refrence_string[i]].is_valid = 1; //sets it to valid so the fact its in memory is marked
        page_table[refrence_string[i]].arrival_timestamp = timestamp;//updates arrival time
        page_table[refrence_string[i]].last_access_timestamp = timestamp;//updates  last access time
        page_table[refrence_string[i]].reference_count = 1;//this is the first time it was referenced the count is 1
        
        //Move to the page no longer in memory all values are set to null since it is no longer in memory

        page_table[smallest_loc].frame_number = -1;
        page_table[smallest_loc].is_valid = 0;
        page_table[smallest_loc].arrival_timestamp = -1;
        page_table[smallest_loc].last_access_timestamp = -1;
        page_table[smallest_loc].reference_count = -1;

        page_fault_counter += 1;
        
    }   
    timestamp += 1; 
}
    return page_fault_counter;
}

int process_page_access_lru(struct PTE page_table[TABLEMAX],
int *table_cnt, 
int page_number, 
int frame_pool[POOLMAX],
int *frame_cnt, 
int current_timestamp)
{
    /*The function determines the memory frame number for the logical page and returns this
number.

First the function checks if the page being referenced is already in memory (i.e., the page-table
entry has the valid bit true). If so, it returns the frame number, after modifying the
last_access_timestamp and the reference_count fields of the page-table entry.*/
   if (page_table[page_number].is_valid !=0 ){
       page_table[page_number].reference_count += 1; //update reference count
       page_table[page_number].last_access_timestamp = current_timestamp; //update time
       return page_table[page_number].frame_number;//returns frame number
    }

/*If the page being referenced is not in memory, the function checks if there are any free frames
(i.e., the process frame pool is not empty). If so, a frame is removed from the process frame
pool and the frame number is inserted into the page-table entry corresponding to the logical
page number. In addition, the other fields of the page-table entry are set appropriately. The
function returns the frame number associated with the page-table entry.*/
    else if(page_table[page_number].is_valid == 0 && *frame_cnt > 0){
        *frame_cnt -= 1; //lowers frame count
        page_table[page_number].frame_number = frame_pool[*frame_cnt];//updates the number of free frames
        page_table[page_number].reference_count = 1; //update reference count
        page_table[page_number].last_access_timestamp = current_timestamp; //update last access time
        page_table[page_number].arrival_timestamp = current_timestamp;//just moved to memory so new arrival time
        page_table[page_number].is_valid = 1; //moved to memory so it is valid
        return page_table[page_number].frame_number;
    }
/*If the page being referenced is not in memory and there are no free frames for the process, a
page needs to be replaced. The function selects among all the pages of the process that are
currently in memory (i.e., they have valid bits as true) the page that has the smallest
reference_count. If multiple pages have the smallest reference_count, the one with the smallest
arrival_timestamp among these is selected. After selecting the page for replacement, the
function marks that page-table entry as invalid, and sets the frame_number, arrival_timestamp,
last_access_timestamp and reference_count to -1. It then sets the frame_number of the
page-table entry of the newly-referenced page to the newly freed frame. It also sets the
arrival_timestamp, the last_access_timestamp and the reference_count fields of the page-table
entry appropriately. Finally, the function returns this frame number.
*/
    else {
        int smallest_access_timestamp = INT_MAX;//potential bug here using location 0 in table
        int smallest_loc = 0;

        for(int i = 0; i < *table_cnt; i++){
            if(page_table[i].is_valid != 0) {
                if(page_table[i].last_access_timestamp < smallest_access_timestamp){
                    smallest_access_timestamp = page_table[i].last_access_timestamp;
                    smallest_loc = i;
                }

            }
        }
        page_table[page_number].frame_number = page_table[smallest_loc].frame_number;
        page_table[page_number].is_valid = 1;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;

        page_table[smallest_loc].frame_number = -1;
        page_table[smallest_loc].is_valid = 0;
        page_table[smallest_loc].arrival_timestamp = -1;
        page_table[smallest_loc].last_access_timestamp = -1;
        page_table[smallest_loc].reference_count = -1;
        return page_table[page_number].frame_number;
    }

    
}
int count_page_faults_lru(struct PTE page_table[TABLEMAX],
int table_cnt,
int refrence_string[REFERENCEMAX],
int reference_cnt,
int frame_pool[POOLMAX],
int frame_cnt)
{
    int page_fault_counter = 0;
    int timestamp = 1;
    /*The function returns the estimated number of page faults for the reference string, with respect to
the pool of frames allocated to the process. For each logical page number (in the sequence),
the function simulates the processing of the page access in the LFU system. It keeps track of
the number of page faults that occur in the system as it simulates the processing of the entire
sequence of logical page numbers.*/

/*When simulating the processing of a page access, the function first checks if the page being
referenced is already in memory (i.e., the page-table entry has the valid bit true). If so, it
modifies the last_access_timestamp and the reference_count fields of the page-table entry. It
then moves on to the processing of the next entry in the sequence of logical page numbers.
In order to simulate timestamps, the function starts with a timestamp of 1 and increments it
whenever the processing of a new page access is begun.*/
    for(int i = 0; i < reference_cnt; i++){ 
        if (page_table[refrence_string[i]].is_valid !=0 ){
            page_table[refrence_string[i]].reference_count += 1; //update reference count
            page_table[refrence_string[i]].last_access_timestamp = timestamp; //update time
        }

/*If the page being referenced is not in memory, the function checks if there are any free frames
(i.e., the process frame pool is not empty). If so, a frame is removed from the process frame
pool and the frame number is inserted into the page-table entry corresponding to the logical
page number. In addition, the other fields of the page-table entry are set appropriately. The
function counts this page access as a page fault.*/
        else if(page_table[refrence_string[i]].is_valid == 0 && frame_cnt > 0){
            frame_cnt -= 1; //lowers frame count
            page_table[refrence_string[i]].frame_number = frame_pool[frame_cnt];//updates the number of free frames
            page_table[refrence_string[i]].reference_count = 1; //update reference count
            page_table[refrence_string[i]].last_access_timestamp = timestamp; //update last access time
            page_table[refrence_string[i]].arrival_timestamp = timestamp;//just moved to memory so new arrival time
            page_table[refrence_string[i]].is_valid = 1; //moved to memory so it is valid
            page_fault_counter += 1;
            timestamp += 1;
        }

/*If the page being referenced is not in memory and there are no free frames for the process, a
page needs to be replaced. The function selects among all the pages of the process that are
currently in memory (i.e., they have valid bits as true) the page that has the smallest
reference_count. If multiple pages exist with the smallest reference_count, the one with the
smallest arrival_timestamp is chosen. The function then marks that page-table entry as invalid,
along with setting the arrival_timestamp, last_access_timestamp and reference_count to 0. It
then sets the frame_number of the page-table entry of the newly-referenced page to the newly
freed frame. It also sets the arrival_timestamp, the last_access_timestamp and the
reference_count fields of the page-table entry appropriately. The function counts this page
access as a page fault.
The function returns the total number of page faults encountered in the simulation*/
    else {
        int smallest_access_timestamp = INT_MAX;//potential bug here using location 0 in table
        int smallest_loc = 0;

        for(int i = 0; i < table_cnt; i++){
            if(page_table[i].is_valid != 0) {
                if(page_table[i].last_access_timestamp < smallest_access_timestamp){
                    smallest_access_timestamp = page_table[i].last_access_timestamp;
                    smallest_loc = i;
                }

            }
        }
        page_table[refrence_string[i]].frame_number = page_table[smallest_loc].frame_number;
        page_table[refrence_string[i]].is_valid = 1;
        page_table[refrence_string[i]].arrival_timestamp = timestamp;
        page_table[refrence_string[i]].last_access_timestamp = timestamp;
        page_table[refrence_string[i]].reference_count = 1;

        page_table[smallest_loc].frame_number = -1;
        page_table[smallest_loc].is_valid = 0;
        page_table[smallest_loc].arrival_timestamp = -1;
        page_table[smallest_loc].last_access_timestamp = -1;
        page_table[smallest_loc].reference_count = -1;

        page_fault_counter += 1;
        timestamp += 1;
    }
    }
    return page_fault_counter;
    //fin
}