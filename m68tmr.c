#include "m68tmr.h"
#include <assert.h>
#include <stdio.h>



#define FIXED_PRESCALER 6

enum{
  TCR_RESPRE=3,
  TCR_EXTEN=4,
  TCR_EXTSRC=5,
  TCR_INTDIS=6,
  TCR_INTREQ=7
};


enum{ADDR_TIMERDATA=0x0008,
     ADDR_TIMERCTRL=0x0009
    };


void reset_prescaler(M68TMR_CTX* ctx){
#ifdef FIXED_PRESCALER
  uint8_t preshift=FIXED_PRESCALER;
#else
  uint8_t preshift=ctx->tcr&0x07;
#endif    
  ctx->prescaler=(1<<preshift) -1;
}
 
void tmr_init(M68TMR_CTX* ctx){
  ctx->tcr=
    (0<<TCR_INTREQ)|
    (1<<TCR_INTDIS);
  ctx->tdr=0xFF;
  reset_prescaler(ctx);
}

bool tmr_exec(M68TMR_CTX* ctx, uint64_t cycles,bool pin){

  uint8_t mode= (ctx->tcr>>TCR_EXTEN)&0x03;
#ifdef FIXED_PRESCALER  
  uint8_t preshift=FIXED_PRESCALER;
#else
  uint8_t preshift=ctx->tcr&0x07;
#endif    
  uint64_t count;
  switch (mode){
  case 0:
    count=cycles;
    break;
  default:
    assert(1==2);//not implemented yet
  }

  

  uint8_t n=count>>preshift;
  if (count>ctx->prescaler){
    n=1;
  }
  ctx->prescaler=(ctx->prescaler-count)&((1<<preshift) -1);

  bool interrupt= n>=ctx->tdr;
  ctx->tdr=(ctx->tdr-n  )&0xFF;
  
  if (interrupt){
    ctx->tcr|=1<<TCR_INTREQ;
  }
  // printf("count %ld prescaler %d tdr %d n%d\n",count,ctx->prescaler,ctx->tdr,n);

  
  return interrupt & ( (ctx->tcr& (1<<TCR_INTDIS))==0);
}


void tmr_read(M68TMR_CTX* ctx,uint16_t addr,uint8_t* val){
  switch (addr){
  case ADDR_TIMERDATA:
    *val=ctx->tdr;
    break;
  case ADDR_TIMERCTRL:
    *val=ctx->tcr;
    break;
  default:
    break;
  }
}
                                                          
void tmr_write(M68TMR_CTX* ctx,uint16_t addr,uint8_t val){
  switch (addr){
  case ADDR_TIMERDATA:
    ctx->tdr=val;
    //printf("count set to %d \n",val);
    break;
  case ADDR_TIMERCTRL:
    ctx->tcr=val;
    if (val& (1<<TCR_RESPRE)){
      reset_prescaler(ctx);
    }
    break;
  default:
    break;
  }
}


