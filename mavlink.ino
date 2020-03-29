#include "variables.h"

#include "src/mavlink/mavlink.h"

#if defined(DEBUG_MODE)
void debugMavlink(bool tx, const mavlink_message_t* msg) {
  char buf1[100];
  if (tx) {
    snprintf(buf1, 100, "SEND ID: %d, SEQ: %d, COMP: %d, SYS: %d, LEN: %d", msg->msgid, msg->seq, msg->compid, msg->sysid, len);
  }
  else {
    snprintf(buf1, 100, "RECV ID: %d, SEQ: %d, COMP: %d, SYS: %d, LEN: %d", msg->msgid, msg->seq, msg->compid, msg->sysid, len);
  }
  DEBUG_PORT.println(buf1);  
}
#endif

#ifdef SEND_HEARTBEAT
uint16_t heartbeat(uint8_t *buf) {
  mavlink_message_t msg;

  mavlink_msg_heartbeat_pack(MAV_SYSTEM_ID, 0, &msg, MAV_TYPE_GCS /*6*/, MAV_AUTOPILOT_INVALID /*8*/, 0, 0, 0);
  #if defined(DEBUG_MODE)
  debugMavlink(true, &msg);
  #endif
  return mavlink_msg_to_send_buffer(buf, &msg);
}
#endif
