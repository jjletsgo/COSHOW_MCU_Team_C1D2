
#ifndef ADC_H_
#define ADC_H_

void ADC_init(void);
void ADC_select_channel(unsigned char channel);
int read_ADC(void);


#endif /* ADC_H_ */