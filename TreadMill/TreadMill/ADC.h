
#ifndef ADC_H_
#define ADC_H_

void ADC_init(void);
void ADC_select_channel(unsigned char channel);
int read_ADC(void);

uint16_t ADC_value;

#endif /* ADC_H_ */