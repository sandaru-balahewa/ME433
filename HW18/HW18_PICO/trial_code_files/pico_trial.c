    while (true) {
        uint16_t angle       = read_raw_angle();
        float force_raw   = (float)hx711_read_raw();
        
        // Angular velocity estimate
        static uint16_t prev_angle = 0;

        // encoder wraps at 4096 counts
        int32_t delta_angle = (int32_t)angle - (int32_t)prev_angle;

        if (delta_angle > 2048)  delta_angle -= 4096;
        if (delta_angle < -2048) delta_angle += 4096;

        float angle_velocity = (float)delta_angle / dt;   // counts/sec

        prev_angle = angle;

        // Force filtering
        // low pass filter the force as it's noisy
        static float force_filt = 0.0f;

        force_filt = 0.2*force_filt + 0.8*force_raw;
        float actual_force = force_filt;

        // Haptic wall logic
        float desired_force  = compute_desired_force(angle);

        // LEFT SIDE:
        // assist if user is moving right (toward center)
        if (angle <= ANGLE_WALL_LOW) {

            if (angle_velocity > 5.0f) {
                desired_force = ASSIST_FORCE;
            }
        }

        // RIGHT SIDE:
        // assist if user is moving left (toward center)
        else if (angle >= ANGLE_WALL_HIGH) {

            if (angle_velocity < -5.0f) {
                desired_force = -ASSIST_FORCE;
            }
        }

        float error = desired_force - actual_force;

        if (error > 0){
            error -= 3000;
        }
        else{
            error += 3000;
        }

        if (fabsf(error) < 1500.0f){
            error = 0.0f;
        }
        float derivative = (error - prev_error) / dt;
        float desired_current_ma = KP * error + KD * derivative;
        prev_error = error;
 
        // Clamp
        if (desired_current_ma >  CURRENT_MAX_MA) desired_current_ma =  CURRENT_MAX_MA;
        if (desired_current_ma < -CURRENT_MAX_MA) desired_current_ma = -CURRENT_MAX_MA;
 
        // // Send to STM32 over UART
        // uart_send_float(desired_current_ma);

        // Send desired current to STM32 using CAN
        bool acked = can_send_float(CAN_ID, desired_current_ma);
 
        // if (!acked) {
        //     printf("CAN no ACK\n");
        // }
 
        // Debug to serial (comment out if too slow)
        printf("angle=%4u  f_des=%7.1f  f_act=%7.1f  i_des=%7.1f mA\n",
               angle, desired_force, actual_force, desired_current_ma);
 
        sleep_ms(10);   // ~100 Hz