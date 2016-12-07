/* Biomedical Image Analysis Library
 * See README file in the root instalation directory for more information.
 */

/**
 * @date 2012/Sep/19
 * @brief Oriented intern Path function to compute the initial path-values and propagated path-values in IFT algorithm.
 *        Function for image segmentation with geodesic star distance. Result is normaly based on label map.
 */

#ifndef BIALORIENTEDINTERNPATHFUNCTION_C
#define BIALORIENTEDINTERNPATHFUNCTION_C

#include "OrientedInternPathFunction.hpp"

#if defined ( BIAL_EXPLICIT_LIB ) && ( BIAL_OrientedInternPathFunction )
#define BIAL_EXPLICIT_OrientedInternPathFunction
#endif
#if defined ( BIAL_EXPLICIT_OrientedInternPathFunction ) || ( BIAL_IMPLICIT_BIN )

#include "BucketQueueElements.hpp"
#ifdef BIAL_DEBUG
#include "FileImage.hpp"
#endif

/* Implementation ***************************************************************************************************** */

namespace Bial {

  template< class D >
  OrientedInternPathFunction< D >::OrientedInternPathFunction( const Image< D > &handicap, 
                                                               const Image< D > &new_intensity,
                                                               Image< int > *restriction, double new_alpha ) try :
    PathFunction< Image, D >( ), intensity( new_intensity ), handicap( handicap ), geodesic_restriction( restriction ),
      alpha( new_alpha ) {
      if( ( geodesic_restriction != nullptr ) && ( intensity.Dims( ) != restriction->Dims( ) ) ) {
        std::string msg( BIAL_ERROR( "Image dimensions do not match." ) );
        throw( std::logic_error( msg ) );
      }
      if( ( alpha < 0.0 ) || ( alpha > 1.0 ) ) {
        std::string msg( BIAL_ERROR( "Invalid alpha. Expected: 0.0 to 1.0. Given: " + std::to_string( alpha ) + "." ) );
        throw( std::logic_error( msg ) );
      }
    }
  catch( std::bad_alloc &e ) {
    std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Memory allocation error." ) );
    throw( std::runtime_error( msg ) );
  }
  catch( std::runtime_error &e ) {
    std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Runtime error." ) );
    throw( std::runtime_error( msg ) );
  }
  catch( const std::out_of_range &e ) {
    std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Out of range exception." ) );
    throw( std::out_of_range( msg ) );
  }
  catch( const std::logic_error &e ) {
    std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Empty kernel." ) );
    throw( std::logic_error( msg ) );
  }

  template< class D >
  OrientedInternPathFunction< D >::OrientedInternPathFunction( const OrientedInternPathFunction< D > &pf ) try :
    OrientedInternPathFunction< D >( pf.handicap, pf.intensity, pf.geodesic_restriction, pf.alpha ) {
    }
  catch( std::bad_alloc &e ) {
    std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Memory allocation error." ) );
    throw( std::runtime_error( msg ) );
  }
  catch( std::runtime_error &e ) {
    std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Runtime error." ) );
    throw( std::runtime_error( msg ) );
  }
  catch( const std::out_of_range &e ) {
    std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Out of range exception." ) );
    throw( std::out_of_range( msg ) );
  }
  catch( const std::logic_error &e ) {
    std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Empty kernel." ) );
    throw( std::logic_error( msg ) );
  }

  template< class D > OrientedInternPathFunction< D >::~OrientedInternPathFunction( ) {
  }

  template< class D > OrientedInternPathFunction< D >
  OrientedInternPathFunction< D >::operator=( const OrientedInternPathFunction< D > &pf ) {
    try {
      return( OrientedInternPathFunction< D >( pf ) );
    }
    catch( std::bad_alloc &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Memory allocation error." ) );
      throw( std::runtime_error( msg ) );
    }
    catch( std::runtime_error &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Runtime error." ) );
      throw( std::runtime_error( msg ) );
    }
    catch( const std::out_of_range &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Out of range exception." ) );
      throw( std::out_of_range( msg ) );
    }
    catch( const std::logic_error &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Logic Error." ) );
      throw( std::logic_error( msg ) );
    }
  }

  template< class D >
  void OrientedInternPathFunction< D >::Initialize( Image< D > &init_value, Image< int > *init_label,
                                                    Image< int > *init_predecessor, bool sequential_label ) {
    try {
      if( ( geodesic_restriction != nullptr ) && ( init_value.Dims( ) != geodesic_restriction->Dims( ) ) ) {
        std::string msg( BIAL_ERROR( "Image dimensions do not match." ) );
        throw( std::logic_error( msg ) );
      }
      if( init_label == nullptr ) {
        std::string msg( BIAL_ERROR( "This path function requires label map." ) );
        throw( std::logic_error( msg ) );
      }
      if( intensity.Dims( ) != init_value.Dims( ) ) {
        std::string msg( BIAL_ERROR( "Image dimensions do not match." ) );
        throw( std::logic_error( msg ) );
      }
      if( init_label == nullptr ) {
        std::string msg( BIAL_ERROR( "This path-function requires label map." ) );
        throw( std::logic_error( msg ) );
      }
      PathFunction< Image, D >::Initialize( init_value, init_label, init_predecessor, sequential_label );
    }
    catch( std::bad_alloc &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Memory allocation error." ) );
      throw( std::runtime_error( msg ) );
    }
    catch( std::runtime_error &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Runtime error." ) );
      throw( std::runtime_error( msg ) );
    }
    catch( const std::out_of_range &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Out of range exception." ) );
      throw( std::out_of_range( msg ) );
    }
    catch( const std::logic_error &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Logic Error." ) );
      throw( std::logic_error( msg ) );
    }
  }

  template< class D >
  bool OrientedInternPathFunction< D >::RemoveSimple( size_t, BucketState ) {
    return( true );
  }

  template< class D >
  bool OrientedInternPathFunction< D >::RemoveLabel( size_t index, BucketState state ) {
    try {
      if( state == BucketState::INSERTED ) {
        this->label->operator()( index ) = this->next_label;
        ++( this->next_label );
      }
      return( true );
    }
    catch( std::bad_alloc &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Memory allocation error." ) );
      throw( std::runtime_error( msg ) );
    }
    catch( std::runtime_error &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Runtime error." ) );
      throw( std::runtime_error( msg ) );
    }
    catch( const std::out_of_range &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Out of range exception." ) );
      throw( std::out_of_range( msg ) );
    }
    catch( const std::logic_error &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Logic Error." ) );
      throw( std::logic_error( msg ) );
    }
  }

  template< class D >
  inline bool OrientedInternPathFunction< D >::Capable( int index, int adj_index, BucketState adj_state ) {
    try {
      return( ( adj_state != BucketState::REMOVED ) && 
              ( this->value->operator()( index ) < this->value->operator()( adj_index ) ) );
    }
    catch( std::bad_alloc &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Memory allocation error." ) );
      throw( std::runtime_error( msg ) );
    }
    catch( std::runtime_error &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Runtime error." ) );
      throw( std::runtime_error( msg ) );
    }
    catch( const std::out_of_range &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Out of range exception." ) );
      throw( std::out_of_range( msg ) );
    }
    catch( const std::logic_error &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Logic Error." ) );
      throw( std::logic_error( msg ) );
    }
  }

  template< class D >
  bool OrientedInternPathFunction< D >::Propagate( int index, int adj_index ) {
    try {
      D src_value = this->value->operator()( adj_index );
      COMMENT( "Computing arc weight.", 3 );
      double arc_weight = handicap( index ) + handicap( adj_index );
      COMMENT( "Orienting edges.", 3 );
      double fraction = 0;
      if( intensity[ index ] > intensity[ adj_index ] ) {
        fraction = alpha;
      }
      else if( intensity[ index ] < intensity[ adj_index ] ) {
        fraction = -alpha;
      }
      if( this->label->operator()( index ) != 0 ) {
        fraction = -fraction;
      }
      arc_weight = std::round( arc_weight * ( 1.0 + fraction ) );
      COMMENT( "Suppressing non-zero.", 3 );
      ++arc_weight;
      COMMENT( "Zero weight edges.", 3 );
      if( geodesic_restriction != nullptr ) {
        if( ( ( this->label->operator()( index ) != 0 ) && ( ( *geodesic_restriction )[ index ] == adj_index ) ) ||
            ( ( this->label->operator()( index ) == 0 ) && ( ( *geodesic_restriction )[ adj_index ] == index ) ) ) {
          arc_weight = 0;
        }
      }
      ++arc_weight;
      COMMENT( "Propagated value.", 3 );
      D prp_value = static_cast< D >( arc_weight );
      COMMENT( "Updating path value.", 3 );
      if( src_value > prp_value ) {
        this->value->operator()( adj_index ) = prp_value;
        return( true );
      }
      return( false );
    }
    catch( std::bad_alloc &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Memory allocation error." ) );
      throw( std::runtime_error( msg ) );
    }
    catch( std::runtime_error &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Runtime error." ) );
      throw( std::runtime_error( msg ) );
    }
    catch( const std::out_of_range &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Out of range exception." ) );
      throw( std::out_of_range( msg ) );
    }
    catch( const std::logic_error &e ) {
      std::string msg( e.what( ) + std::string( "\n" ) + BIAL_ERROR( "Logic Error." ) );
      throw( std::logic_error( msg ) );
    }
  }

  template< class D >
  bool OrientedInternPathFunction< D >::Increasing( ) {
    return( true );
  }

#ifdef BIAL_EXPLICIT_OrientedInternPathFunction

  template class OrientedInternPathFunction< int >;
  template class OrientedInternPathFunction< llint >;
  template class OrientedInternPathFunction< float >;
  template class OrientedInternPathFunction< double >;

#endif

}

#endif

#endif
