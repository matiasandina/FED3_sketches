#include "FED3.h"  // Include the FED3 library
#include <SdFat.h>


String sketch = "VarIBI";  // Unique identifier text for this sketch
FED3 fed3(sketch);  // Start the FED3 object

// Experiment parameters
int ipi_device = 3;  // Inter-pellet interval set by the device in seconds
int max_pellets_per_day = 250;  // Maximum number of pellets to be dispensed in a day
int cumulative_pellets = 0;  // To keep track of the total dispensed pellets for the current day

// Bout and IBI configuration
int bout_duration_options[2] = {60, 30};  // Bout duration options in seconds
int ibi_options[3] = {20, 15, 30};  // IBI options in seconds

// Active and rest phase configuration
int active_phase_start = 19;  // Active phase starts at 19:00 (7 PM)
int active_phase_end = 7;    // Active phase ends at 07:00 (7 AM) the next day

// Pellets per bout configuration
int pellets_per_bout_active[2] = {10, 20};  // During active phase
int pellets_per_bout_rest[1] = {5};  // During rest phase

// Global variable to remember the last day we operated
static int last_day = -1;  // Initialize with an invalid value to ensure proper setup

//Variables for logging bouts
char boutFileName[45];
File boutFile;
DateTime bout_start_dt;
DateTime bout_stop_dt;
int bout_number = 1; // Example bout number


void setup() {
  fed3.begin();  // Setup the FED3 hardware
  createBoutLogFile();
  fed3.DisplayPokes = false;  // Not displaying the poke indicators
  fed3.timeout = ipi_device;  // Timeout period after each pellet is taken
  Serial.begin(9600); // Start the serial communication with a baud rate of 9600

}


void loop() {

  fed3.run();
  DateTime now = fed3.now();  // Simplified way to get the current time
  int current_hour = now.hour();
  int current_day = now.day();

  Serial.println("Today datetime is");

  Serial.println(now.month()); Serial.println(", ");
  Serial.println(now.day()); Serial.println(", ");
  Serial.println(now.year()); Serial.println(", ");
  Serial.println(now.hour()); Serial.println(", ");
  Serial.println(now.minute()); Serial.println(", ");
  Serial.print(now.second());


  // Reset cumulative pellets at the start of each new day
  if (current_day != last_day) {
    Serial.println("New day has started, resetting pellet count.");
    cumulative_pellets = 0;  // Reset daily pellet count for the new day
    last_day = current_day;  // Update the remembered day
  }

  if (cumulative_pellets < max_pellets_per_day) {
    Serial.println(F("Starting a new bout..."));
    int pellets_per_bout = determinePelletsPerBout(current_hour);
    int max_pellets_per_bout = pellets_per_bout;
    int bout_duration = sampleBoutDuration();
    unsigned long bout_end_time = millis() + bout_duration * 1000;  // Calculate bout end time

    Serial.print("Pellets per bout: "); Serial.println(pellets_per_bout);
    Serial.print("Bout duration (seconds): "); Serial.println(bout_duration);
    Serial.print("Bout end time (millis): "); Serial.println(bout_end_time);


    // bool dispenseEvent = false; // moved up here to initialize
    int pellets_before_feed;

    int pellets_after_feed;

    bout_start_dt = fed3.now();

    // Dispense pellets during the bout
    while (millis() < bout_end_time && cumulative_pellets < max_pellets_per_day) {

      Serial.print("Bout duration (seconds): "); Serial.println(bout_duration);
      if (pellets_per_bout > 0) {

        pellets_before_feed = fed3.PelletCount;

        // // bool dispenseEvent = false;
        // // if (fed3.feedStateToString()== "Dispensing"){
        // //   dispenseEvent = true;
        // // }
        // if (fed3.feedStateToString() == "Logging" and !fed3.IsWellEmpty()){
        //   dispenseEvent = true;
        // }
        // // else{
        // //   dispenseEvent = false;
        // // }

        fed3.FeedNonBlocking();

        pellets_after_feed = fed3.PelletCount;

        // fed3.Feed(skipRemovalCheck=false, blockUntilRemoval=false);  // Dispense a pellet do not hang in the while loop

        if (pellets_before_feed != pellets_after_feed){ //check pellet event before updating
          pellets_per_bout --;
          cumulative_pellets++;
          Serial.print("Pellets Left in bout: "); Serial.println(pellets_per_bout);
          delay(2);
        }



        Serial.print("Pellets Left in bout: "); Serial.println(pellets_per_bout);
        Serial.print("Total Pellets: "); Serial.println(cumulative_pellets);

      }
    }
    Serial.println("Bout ended.");

    // Wait until bout end time, if necessary, before starting the IBI
    while (millis() < bout_end_time) {
      // Idle here until the bout officially ends
    }

    bout_stop_dt = fed3.now();
    int ibi = sampleIBI();
    unsigned long next_bout_start = millis() + ibi * 1000;  // Calculate next bout start

    writeBoutMetadata(bout_number, bout_start_dt, bout_stop_dt, bout_end_time, max_pellets_per_bout, ibi);
    bout_number ++;

    Serial.print(F("Inter-bout interval (seconds): ")); Serial.println(ibi);
    Serial.print(F("Next bout start time (millis): ")); Serial.println(next_bout_start);

        // TODO:
    // bout_start_dt = fed3.now()
    // bout_stop_dt = bout_start_dt + seconds(bout_duration)
    // bout_number++
    // store bout_number, bout_start_dt, bout_stop_dt, pellets_per_bout into a separate csv file
    // ideally we match the session name for the FED csv with events FED[0-9]+_mmddyy_sesion_n.csv
    // e.g., "FED[]_mmddyy_session_n_bout_metadata.csv"]


    // Idle until it's time for the next bout
    while (millis() < next_bout_start) {
      // Idle here until the IBI passes
    }
    Serial.println("Inter-bout interval ended. Starting next bout...");
  } else{
    Serial.println("Max pellets for today have been dispensed.");
  }
}

// Helper function implementations

bool isInActivePhase(int current_hour) {
  if (active_phase_start < active_phase_end) {
    return current_hour >= active_phase_start && current_hour < active_phase_end;
  } else {
    return current_hour >= active_phase_start || current_hour < active_phase_end;
  }
}

int determinePelletsPerBout(int current_hour) {
  if (isInActivePhase(current_hour)) {
    return randomValueFromArray(pellets_per_bout_active, sizeof(pellets_per_bout_active) / sizeof(int));
  } else {
    return randomValueFromArray(pellets_per_bout_rest, sizeof(pellets_per_bout_rest) / sizeof(int));
  }
}

int sampleBoutDuration() {
  return randomValueFromArray(bout_duration_options, sizeof(bout_duration_options) / sizeof(int));
}

int sampleIBI() {
  return randomValueFromArray(ibi_options, sizeof(ibi_options) / sizeof(int));
}

int randomValueFromArray(int arr[], int arrSize) {
  return arr[random(0, arrSize)];
}



void createBoutLogFile() {
  // Initialize SD card
  if (!fed3.SD.begin(4, SD_SCK_MHZ(4))) {
    Serial.println("Error initializing SD card");
    return;
  }

  // Ensure the original filename ends with ".csv"
  size_t len = strlen(fed3.filename);
  if (len < 4 || strcmp(&fed3.filename[len - 4], ".CSV") != 0) {
    Serial.println("Original filename does not end with .CSV");
    return;
  }

  // Create new filename by replacing ".CSV" with "_bout_metadata.csv"
  strncpy(boutFileName, fed3.filename, len - 4); // Copy up to but not including ".CSV"
  boutFileName[len - 4] = '\0'; // Null-terminate the string
  strcat(boutFileName, "_bout_metadata.csv"); // Append the new extension

  // Create or open the bout log file
  boutFile = fed3.SD.open(boutFileName, FILE_WRITE);
  if (!boutFile) {
    Serial.print("Error opening bout log file: ");
    Serial.println(boutFileName);
    return;
  }

  Serial.print("Opened bout log file: ");
  Serial.println(boutFileName);

  boutFile.println("Bout_Number, Bout_Start, Bout_Stop, Bout_End, Pellets_Per_Bout, IBI");
  // Close the file after operation
  boutFile.close();
}


void writeBoutMetadata(int boutNumber, DateTime start, DateTime stop, unsigned long bout_end_time, int pelletsPerBout, int ibi) {
    // // Open the bout metadata file for writing
    Serial.println(boutFileName);
    boutFile = fed3.SD.open(boutFileName, FILE_WRITE);  // Assuming boutFileName is provided

    if (!boutFile) {
        Serial.println("Error opening bout metadata file!");
        return;
    }

    // Write the bout metadata header if the file is empty
    // if (boutFile.size() == 0) {
    //     boutFile.println("Bout_Number,Start_DateTime,Stop_DateTime,Pellets_Per_Bout, IBI");
    // }

    // Format and write the bout metadata
    char buffer[160];
    sprintf(buffer, "%d,%04d%02d%02dT%02d:%02d:%02d,%04d%02d%02dT%02d:%02d:%02d,%lu,%d,%d",
            boutNumber,
            start.year(), start.month(), start.day(), start.hour(), start.minute(), start.second(),
            stop.year(), stop.month(), stop.day(), stop.hour(), stop.minute(), stop.second(),
            bout_end_time,
            pelletsPerBout,
            ibi);

    boutFile.println(buffer);
    boutFile.flush();
    boutFile.close();
}
