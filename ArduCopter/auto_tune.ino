/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#if AUTOTUNE == ENABLED
/*
    Auto tuning works in this way:
        i) set up 3-position ch7 or ch8 switch to "AutoTune"
        2) take-off in stabilize mode, put the copter into a level hover and switch ch7/ch8 to high position to start tuning
        3) auto tuner brings roll and pitch level
        4) the following procedure is run for roll and then pitch
            a) invokes 90 deg/sec rate request
            b) records maximum "forward" roll rate and bounce back rate
            c) when copter reaches 20 degrees or 1 second has passed, it commands level
            d) tries to keep max rotation rate between 80% ~ 100% of requested rate (90deg/sec) by adjusting rate P
            e) increases rate D until the bounce back becomes greater than 10% of requested rate (90deg/sec)
            f) decreases rate D until the bounce back becomes less than 10% of requested rate (90deg/sec)
            g) increases rate P until the max rotate rate becomes greater than the requeste rate (90deg/sec)
            h) invokes a 20deg angle request on roll or pitch
            i) increases stab P until the maximum angle becomes greater than 110% of the requested angle (20deg)
            j) decreases stab P by 25%
    If pilot inputs any stick inputs these becomes the desired roll, pitch angles sent to the stabilize controller and the tuner is disabled until the sticks are put back into the middle for 1 second
*/

#define AUTO_TUNE_PILOT_OVERRIDE_TIMEOUT_MS  500    // restart tuning if pilot has left sticks in middle for 2 seconds
#define AUTO_TUNE_TARGET_RATE_TIMEOUT_MS     500    // timeout for rate test step
#define AUTO_TUNE_TARGET_RATE_CDS           9000    // target roll/pitch rate during AUTO_TUNE_STEP_TESTING step
#define AUTO_TUNE_LEVEL_ANGLE_CD             300    // angle which qualifies as level
#define AUTO_TUNE_TARGET_ANGLE_CD           2000    // target angle during TESTING_RATE step that will cause us to move to next step
#define AUTO_TUNE_REQUIRED_LEVEL_TIME_MS     250    // time we require the copter to be level
#define AUTO_TUNE_AGGRESSIVENESS            0.1f    // tuning for 10% overshoot
#define AUTO_TUNE_RD_STEP                0.0005f    // minimum increment when increasing/decreasing Rate D term
#define AUTO_TUNE_RP_STEP                 0.005f    // minimum increment when increasing/decreasing Rate P term
#define AUTO_TUNE_SP_STEP                   0.5f    // minimum increment when increasing/decreasing Stab P term
#define AUTO_TUNE_SP_BACKOFF               0.75f    // back off on the Stab P tune
#define AUTO_TUNE_PI_RATIO_FOR_TESTING      0.1f    // I is set 10x smaller than P during testing
#define AUTO_TUNE_RP_RATIO_FINAL            1.0f    // I is set 1x P after testing
#define AUTO_TUNE_RD_MIN                  0.004f    // minimum Rate D value
#define AUTO_TUNE_RD_MAX                  0.015f    // maximum Rate D value
#define AUTO_TUNE_RP_MIN                   0.01f    // minimum Rate P value
#define AUTO_TUNE_RP_MAX                   0.25f    // maximum Rate P value
#define AUTO_TUNE_SP_MAX                   15.0f    // maximum Stab P value
#define AUTO_TUNE_SUCCESS_COUNT                4    // how many successful iterations we need to freeze at current gains

// Auto Tune message ids for ground station
#define AUTO_TUNE_MESSAGE_STARTED 0
#define AUTO_TUNE_MESSAGE_SUCCESS 1
#define AUTO_TUNE_MESSAGE_FAILED 2

enum AutoTuneTuneMode {
    AUTO_TUNE_MODE_UNINITIALISED = 0,
    AUTO_TUNE_MODE_TUNING = 1,
    AUTO_TUNE_MODE_TESTING = 2,
    AUTO_TUNE_MODE_FAILED = 3
};

// things that can be tuned
enum AutoTuneAxisType {
    AUTO_TUNE_AXIS_ROLL = 0,
    AUTO_TUNE_AXIS_PITCH = 1
};

// steps performed during tuning
enum AutoTuneStepType {
    AUTO_TUNE_STEP_WAITING_FOR_LEVEL = 0,
    AUTO_TUNE_STEP_TESTING = 1,
    AUTO_TUNE_STEP_UPDATE_GAINS = 2
};

// steps performed during tuning
enum AutoTuneTuneType {
    AUTO_TUNE_TYPE_RD_UP = 0,
    AUTO_TUNE_TYPE_RD_DOWN = 1,
    AUTO_TUNE_TYPE_RP_UP = 2,
    AUTO_TUNE_TYPE_SP_UP = 3
};

// state
struct auto_tune_state_struct {
    AutoTuneTuneMode mode       : 2;    // see AutoTuneTuneMode for what modes are allowed
    uint8_t pilot_override      : 1;    // 1 = pilot is overriding controls so we suspend tuning temporarily
    AutoTuneAxisType axis       : 1;    // see AutoTuneAxisType for which things can be tuned
    uint8_t positive_direction  : 1;    // 0 = tuning in negative direction (i.e. left for roll), 1 = positive direction (i.e. right for roll)
    AutoTuneStepType step       : 2;    // see AutoTuneStepType for what steps are performed
    AutoTuneTuneType tune_type  : 2;    // see AutoTuneTuneType
} auto_tune_state;

// variables
static uint32_t auto_tune_override_time;   // the last time the pilot overrode the controls
static float auto_tune_test_min;           // the minimum angular rate achieved during TESTING_RATE step
static float auto_tune_test_max;           // the maximum angular rate achieved during TESTING_RATE step
static uint32_t auto_tune_timer;           // generic timer variable
static int8_t auto_tune_counter;           // counter for tuning gains
static float orig_roll_rp, orig_roll_ri, orig_roll_rd, orig_roll_sp;     // backup of currently being tuned parameter values
static float orig_pitch_rp, orig_pitch_ri, orig_pitch_rd, orig_pitch_sp; // backup of currently being tuned parameter values
static float tune_roll_rp, tune_roll_rd, tune_roll_sp;     // currently being tuned parameter values
static float tune_pitch_rp, tune_pitch_rd, tune_pitch_sp;  // currently being tuned parameter values

// store current pids as originals
static void auto_tune_initialise()
{

}

// auto_tune_intra_test_gains - gains used between tests
static void auto_tune_intra_test_gains()
{

}

// auto_tune_restore_orig_gains - restore pids to their original values
static void auto_tune_restore_orig_gains()
{

}

// auto_tune_load_tuned_pids - restore pids to their tuned values
static void auto_tune_load_tuned_gains()
{

}

// auto_tune_load_test_gains - load the to-be-tested gains for a single axis
static void auto_tune_load_test_gains()
{

}

// start an auto tuning session
// returns true if we should change the roll-pitch mode to ROLL_PITCH_AUTOTUNE
// To-Do: make autotune a flight mode so that this slightly non-intuitive returning of a flag is not required
static bool auto_tune_start()
{
    bool requires_autotune_rp_mode = false;
    // tell caller we require roll-pitch mode to be changed to ROLL_PITCH_AUTOTUNE
    return requires_autotune_rp_mode;
}

// turn off tuning and return to standard pids
static void auto_tune_stop()
{

}

// save discovered gains to eeprom if auto tuner is enabled (i.e. switch is in middle or high position)
static void auto_tune_save_tuning_gains_and_reset()
{
    
}

// send message to ground station
void auto_tune_update_gcs(uint8_t message_id)
{

}

// Auto tuning roll-pitch controller
static void
get_autotune_roll_pitch_controller(int16_t pilot_roll_angle, int16_t pilot_pitch_angle, int16_t pilot_yaw_command)
{
    
}
#endif  // AUTOTUNE == ENABLED
