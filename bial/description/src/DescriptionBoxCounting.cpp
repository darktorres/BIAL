#include "DescriptionBoxCounting.hpp"

#include "Adjacency.hpp"
#include "GeometricsScale.hpp"
#include "Signal.hpp"

#include <fstream>
#include <stdlib.h>


namespace Bial {

  BoxCounting::BoxCounting( FeatureDetector< int > *Fd ) : BoxCounting( Fd->Run( ) ) {
  }

  BoxCounting::BoxCounting( Vector < std::tuple < Image< int >, Image< int >> > detected ) : FeatureExtractor< int, double >(
      detected ) {
  }

  void BoxCounting::SetParameters( ParameterInterpreter *interpreter ) {
  }

  std::string BoxCounting::GetParameters( ParameterInterpreter *interpreter ) {
    return( "" );
  }

  BoxCountingfeature BoxCounting::Run( ) {
    size_t SIZE = 11;

    BoxCountingfeature feat;
    Image< int > img;
    Image< int > scaled_img;
    Image< int > mbb;
    Image< int > sqr_img;

    Image< int > mask;
    Image< int > scaled_mask;


    Features< double > histogram;
    Vector< int > counter;
    for( size_t i = 0; i < this->detected.size( ); ++i ) {
      std::tie( img, mask ) = this->detected[ i ];

      size_t size = 1 << ( SIZE - 1 );
      if( ( img.size( 0 ) > size ) && ( img.size( 1 ) > size ) ) {
        scaled_img = Geometrics::Scale( img, img.size( 0 ) / ( float ) size, img.size( 1 ) / ( float ) size, 1.0 );
        scaled_mask = Geometrics::Scale( mask, mask.size( 0 ) / ( float ) size, mask.size( 1 ) / ( float ) size, 1.0 );
      }
      else if( img.size( 0 ) > size ) {
        scaled_img = Geometrics::Scale( img, img.size( 0 ) / ( float ) size, img.size( 1 ), 1.0 );
        scaled_mask = Geometrics::Scale( mask, mask.size( 0 ) / ( float ) size, mask.size( 1 ), 1.0 );
      }
      else if( img.size( 1 ) > size ) {
        scaled_img = Geometrics::Scale( img, img.size( 0 ), img.size( 1 ) / ( float ) size, 1.0 );
        scaled_mask = Geometrics::Scale( mask, mask.size( 0 ), mask.size( 1 ) / ( float ) size, 1.0 );
      }
      else {
        scaled_img = Image< int >( img );
        scaled_mask = Image< int >( mask );
      }
      mbb = Mbb( scaled_img, scaled_mask );


      /* square image-------------------------------------------------------------- */
      sqr_img = Image< int >( size, size );

      int offset = size / 2 - mbb.size( 0 ) / 2 + sqr_img.size( 0 ) * ( size / 2 - mbb.size( 1 ) / 2 );
      for( size_t y = 0; y < mbb.size( 1 ); y++ ) {
        for( size_t x = 0; x < mbb.size( 0 ); x++ ) {
          if( mask( x, y ) != 0 ) {
            sqr_img [ ( x + offset ) + sqr_img.size( 0 ) * y ] = mbb[ x + mbb.size( 0 ) * y ];
          }
        }
      }
      /* -------------------------------------------------------------------------- */

      /* histogram----------------------------------------------------------------- */
      histogram = Features< double >( SIZE );
      counter = Vector< int >( SIZE );
      for( size_t pot = 1; pot <= size; pot *= 2 ) {
        size_t incx = size / pot;
        size_t incy = size / pot;
        for( size_t x = 0; x < size; x += incx ) {
          for( size_t y = 0; y < size; y += incy ) {
            size_t flag = 0;
            for( size_t xlinha = x; ( ( xlinha < x + incx ) && ( flag == 0 ) ); xlinha++ ) {
              for( size_t ylinha = y; ( ( ylinha < y + incy ) && ( flag == 0 ) ); ylinha++ ) {
                if( sqr_img[ xlinha + sqr_img.size( 0 ) * ylinha ] != 0 ) {
                  flag = 1;
                  int index = ( int ) ( log( pot ) / log( 2 ) );
                  histogram[ index ] = log( ( double ) pot / size );
                  counter[ index ]++;
                }
              }
            }
          }
        }
      }
      for( size_t j = 0; j < SIZE; j++ ) {
        histogram[ j ] = log( counter[ j ] );
      }
      feat.push_back( histogram );
      /* ---------------------------------------------------------- */
    }
    return( feat );
  }
}
