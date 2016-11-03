#include "LUCOLOR.h"

#include "Signal.hpp"
#include "Adjacency.hpp"

#include <fstream>

int LUCOLOR::Log(double  value , double n)
{
  value = 255.0 * value / n;
  if(value==0.)
    return 0;
  else if(value<1.)
    return 1;
  else if(value<2.)
    return 2;
  else if(value<4.)
    return 3;
  else if(value<8.)
    return 4;
  else if(value<16.)
    return 5;
  else if(value<32.)
    return 6;
  else if(value<64.)
    return 7;
  else if(value<128.)
    return 8;
  else
    return 9;
}

LUCOLOR::LUCOLOR(FeatureDetector<Color> *Fd) : LUCOLOR ( Fd->Run() ){ }

LUCOLOR::LUCOLOR(Vector<tuple<Image<Color>,Image<int>>> detected) : FeatureExtractor< Color , int >(detected)
{
  this->dim = 4;
  this->qtd_blocos = 100;
}

void LUCOLOR::SetParameters( ParameterInterpreter* interpreter )
{
  string temp;
  Vector<parameter> vet;
  vet.push_back(tie("dim",dim));
  vet.push_back(tie("qtd_blocos",qtd_blocos));

  interpreter->SetExpectedParameters(vet);
  vet = interpreter->Interpret( );

  tie(ignore , dim) = vet[0];
  tie(ignore , qtd_blocos) = vet[1];
}

string LUCOLOR::GetParameters( ParameterInterpreter* interpreter )
{
  Vector<parameter> vet;
  vet.push_back(tie("dim",dim));
  vet.push_back(tie("qtd_blocos",qtd_blocos));

  interpreter->SetExpectedParameters(vet);

  return interpreter->GetExpectedParameters(  );
}

LUCOLORfeature LUCOLOR::Run( )
{
  size_t size = 3 * this->qtd_blocos;

  LUCOLORfeature feat;
  Image< Color > img;
  Image< int > mask;
  Image< int > quantized;

  size_t nelem, resto;

  double mediaR, mediaG, mediaB;
  double desvioR, desvioG, desvioB;
  long double somaR, somaG, somaB;

  Vector<double> mediaBlocosR;
  Vector<double> mediaBlocosG;
  Vector<double> mediaBlocosB;

  Features<int> histogram;
  Features<int> complete_histogram;

  for(size_t i=0; i<this->detected.size(); ++i)
  {
    tie(img,mask)=this->detected[i];

    nelem = img.size() / qtd_blocos;
    resto = img.size() % qtd_blocos;

    //Calcula media das bandas R, G, B--------------------------------------
    somaR = 0;
    somaG = 0;
    somaB = 0;

    size_t size_soma = 0;

    for(size_t j = 0 ; j<img.size() ; j++)
      if(mask[j]==1)
      {
        somaR += img[j].channel[1];
        somaG += img[j].channel[2];
        somaB += img[j].channel[3];
        size_soma++;
      }

    mediaR = (double)somaR/size_soma;
    mediaG = (double)somaG/size_soma;
    mediaB = (double)somaB/size_soma;
    //----------------------------------------------------------

    //Calculo do desvio padrão das bandas R, G, B--------------------------------------
    somaR = 0;
    somaG = 0;
    somaB = 0;
    size_soma = 0;

    for(size_t j = 0 ; j<img.size() ; j++)
      if(mask[j]==1)
      {
         somaR += pow(img[j].channel[1] - mediaR , 2);
         somaG += pow(img[j].channel[2] - mediaG , 2);
         somaB += pow(img[j].channel[3] - mediaB , 2);
         size_soma++;
      }

   desvioR= sqrt( (somaR / (size_soma -1)) );
   desvioG= sqrt( (somaG / (size_soma -1)) );
   desvioB= sqrt( (somaB / (size_soma -1)) );

    //----------------------------------------------------------

    //Calculo das medias dos blocos--------------------------------------

    mediaBlocosR = Vector<double>(qtd_blocos);
    mediaBlocosG = Vector<double>(qtd_blocos);
    mediaBlocosB = Vector<double>(qtd_blocos);

    int aux = 0;

    for (size_t j = 0; j < qtd_blocos; j++)
    {
      somaR = 0;
      somaG = 0;
      somaB = 0;
      size_soma = 0;

      if(j != qtd_blocos-1)
      {//senao for o ultimo bloco pq ele pode ter menos elementos
          for (size_t k = aux; k < (aux + nelem); k++)
            if(mask[k]==1)
            {
              somaR += img[k].channel[1];
              somaG += img[k].channel[2];
              somaB += img[k].channel[3];
              size_soma++;
            }

          mediaBlocosR[j] = (double)(somaR/size_soma);
          mediaBlocosG[j] = (double)(somaG/size_soma);
          mediaBlocosB[j] = (double)(somaB/size_soma);
      }
      else
      {
          if(resto==0)
            resto = nelem;

          for (size_t k = aux; k < img.size(); k++)
            if(mask[k]==1)
            { //percorrer até final da imagem
                somaR += img[k].channel[1];
                somaG += img[k].channel[2];
                somaB += img[k].channel[3];
                size_soma++;
            }

          mediaBlocosR[j] = (double)(somaR/size_soma);
          mediaBlocosG[j] = (double)(somaG/size_soma);
          mediaBlocosB[j] = (double)(somaB/size_soma);
      }

      aux = aux + nelem;
    }
    //----------------------------------------------------------


    //Histograma------------------------------------------------
    histogram = Features<int>( size + 7 ) ;

    histogram[0] = qtd_blocos;

    histogram[1] = mediaR;
    histogram[2] = mediaG;
    histogram[3] = mediaB;

    histogram[4] = desvioR;
    histogram[5] = desvioG;
    histogram[6] = desvioB;

    for(size_t j = 0; j < qtd_blocos ; j++)
    {

      if(mediaBlocosR[j] > mediaR )//se media do blocoR e maior que a media da imagemR
          histogram[7 + 3*j] = 1;
      else
          histogram[7 + 3*j] = 0;

      if(mediaBlocosG[j] > mediaG )//se media do blocoG e maior que a media da imagemG
          histogram[7 + 3*j + 1] = 1;
      else
          histogram[7 + 3*j + 1] = 0;

      if(mediaBlocosB[j] > mediaB )//se media do blocoB e maior que a media da imagemB
          histogram[7 + 3*j + 2] = 1;
      else
          histogram[7 + 3*j + 2] = 0;
    }

    feat.push_back(histogram);
    //----------------------------------------------------------
  }

  return feat;
}