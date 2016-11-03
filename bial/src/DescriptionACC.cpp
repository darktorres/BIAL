#include "ACC.h"

#include "Signal.hpp"
#include "Adjacency.hpp"

#include <fstream>

int ACC::Log(double  value , double n)
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

ACC::ACC(FeatureDetector<Color> *Fd) : ACC ( Fd->Run() ){ }

ACC::ACC(Vector<tuple<Image<Color>,Image<int>>> detected) : FeatureExtractor< Color , int >(detected)
{
  this->n_distances=4;

  for(size_t r=1; r<=7; r+=2)
  this->distances.push_back(r);

  this->dim = 4;
}

void ACC::SetParameters( ParameterInterpreter* interpreter )
{
  size_t d;
  string temp;
  Vector<parameter> vet;
  vet.push_back(tie("dim",dim));
  vet.push_back(tie("n_distances",n_distances));

  interpreter->SetExpectedParameters(vet);
  vet = interpreter->Interpret( );

  tie(ignore , dim) = vet[0];
  tie(ignore , n_distances) = vet[1];

  vet.clear();

  for(size_t i = 0 ; i<n_distances ; i++)
  {
    temp = string("d")+to_string(i);
    if(i<distances.size())
      vet.push_back(tie( temp ,distances[i]));
    else
    {
      double nd = 0.0;
      vet.push_back(tie( temp , nd ));
    }
  }

  interpreter->SetExpectedParameters(vet);
  vet = interpreter->Interpret( );

  distances.clear();

  for(size_t i = 0 ; i<n_distances ; i++)
  {
    tie(ignore,d) = vet[i];

    distances.push_back(d);
  }

}

string ACC::GetParameters( ParameterInterpreter* interpreter )
{
  Vector<parameter> vet;
  vet.push_back(tie("dim",dim));
  vet.push_back(tie("n_distances",n_distances));
  for(size_t i = 0 ; i<n_distances ; i++)
  {
    string temp = string("d")+to_string(i);
    vet.push_back(tie( temp ,distances[i]));
  }

  interpreter->SetExpectedParameters(vet);

  return interpreter->GetExpectedParameters(  );
}



ACCfeature ACC::Run( )
{
  size_t size = dim * dim * dim * n_distances;

  size_t p;

  ACCfeature feat;
  Image< Color > img;
  Image< int > mask;
  Image< int > quantized;

  unsigned char fator_g = dim;
  unsigned char fator_b = fator_g * dim;
  unsigned char r,g,b;


  Features<int> complete_histogram;
  Vector<int> frequency[n_distances];

  for(size_t i=0; i<this->detected.size(); ++i)
  {
    //quantização------------------------------------------------
    tie(img,mask)=this->detected[i];

    quantized=Image<int>(img.size(0),img.size(1));

    for(size_t j=0; j<quantized.size(  ); ++j)
    {
      r=dim*img[j].channel[1]/256;
      g=dim*img[j].channel[2]/256;
      b=dim*img[j].channel[3]/256;

      quantized[j]=(r + fator_g*g + fator_b*b);
    }
    //-----------------------------------------------------------

    //Frequencia dos pixels--------------------------------------
    for (size_t j = 0; j < n_distances ; j++)
      frequency[j] = Vector<int>(quantized.size());

    Adjacency adjpixels = Adjacency::Circular( 1.1f );

    for(size_t y=0; y<quantized.size(1); y++)
      for(size_t x=0; x<quantized.size(0); x++)
        if(mask(x,y)==1)
        {
          for(size_t d=0; d< n_distances ; d++)
          {
            size_t raio = distances[d];
            p = x + y * img.size(0);

            for( size_t pos = 1; pos < adjpixels.size( ); ++pos )
            {
              int x_dsp = x + raio * adjpixels( pos, 0 );
              int y_dsp = y + raio * adjpixels( pos, 1 );

              if(quantized.ValidCoordinate(x_dsp , y_dsp) && mask(x_dsp , y_dsp)==1  && quantized(x,y)==quantized(x_dsp, y_dsp) && raio!=0)
                frequency[d][p]++;
            }
          }

        }
    //----------------------------------------------------------

    //Histograma------------------------------------------------
    complete_histogram = Features<int>( size ) ;

    for(size_t j=0; j<quantized.size(); j++)
      for(size_t d=0; d<n_distances; d++)
        complete_histogram[ n_distances * quantized[j] + d]+=frequency[d][j];

    for(size_t j=0; j< size ; j++)
        complete_histogram[j]=(int)Log( complete_histogram[j] , n_distances * quantized.size() );

    feat.push_back(complete_histogram);
    //----------------------------------------------------------
  }

  return feat;
}