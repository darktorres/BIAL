/* Biomedical Image Analysis Library */
/* See README file in the root instalation directory for more information. */

/* Author: Fábio Augusto Menocci Cappabianco */
/* Date: 2012/Jun/27 */
/* Version: 1.0.00 */
/* Content: Test file. */
/* Description: Test with Kernel class. */

#include "KernelGaussian.hpp"

using namespace std;
using namespace Bial;

int main( int argc, char **argv ) {
  if( argc != 4 ) {
    cout << "Usage: " << argv[ 0 ] << " <radius> <maximum amplitude> <standard deviation>" << endl;
    cout << "\t\t<radius>: [1.0, 20.0]." << endl;
    cout << "\t\t<maximum amplitude>: (0.0, +oo)." << endl;
    cout << "\t\t<standard deviation>: (0.0,+oo)." << endl;
    return( 0 );
  }
  float radius = atof( argv[ 1 ] );
  float maximum_amplitude = atof( argv[ 2 ] );
  float std_dev = atof( argv[ 3 ] );
  if( ( radius < 1.0 ) || ( radius > 20.0 ) ) {
    cout << "Error: radius range: [1.0, 20.0]. Given: " << radius << endl;
    return( 0 );
  }
  if( maximum_amplitude <= 0.0 ) {
    cout << "Error: maximum amplitude range: (0.0, +oo). Given: " << maximum_amplitude << endl;
    return( 0 );
  }
  if( std_dev <= 0.0 ) {
    cout << "Error: Standard deviation range: (0.0, +oo). Given: " << std_dev << endl;
    return( 0 );
  }
  Kernel krn = KernelType::Gaussian( 2, radius, maximum_amplitude, std_dev );
  for( size_t idx = 0; idx < krn.size( ); ++idx ) {
    cout << "dx: " << krn( idx, 0 ) << ", dy: " << krn( idx, 1 ) << ", value: " << krn.Value( idx ) << endl;
  }
  return( 0 );
}
