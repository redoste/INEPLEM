#ifndef SERVICEMAIN_H
#define SERVICEMAIN_H

#define SERVICE_NAME "INEPLEM"

uint32_t serviceControl(int16_t control);
void serviceMain(int argc, char* argv[]);
void serviceRegister();
#endif