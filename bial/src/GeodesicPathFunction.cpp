/* Biomedical Image Analysis Library
 * See README file in the root instalation directory for more information.
 */

/**
 * @date 2012/Sep/19
 * @brief Geodesic distance path function to compute the initial path-values and propagated path-values in IFT 
 *        algorithm. Used to compute the distance from a set of pixels to the rest of the image.
 */

#ifndef BIALGEODESICPATHFUNCTION_C
#define BIALGEODESICPATHFUNCTION_C

#include "GeodesicPathFunction.hpp"

#if defined ( BIAL_EXPLICIT_LIB ) && ( BIAL_GeodesicPathFunction )
#define BIAL_EXPLICIT_GeodesicPathFunction
#endif
#if defined ( BIAL_EXPLICIT_GeodesicPathFunction ) || ( BIAL_IMPLICIT_BIN )

#include "BucketQueueElements.hpp"
#include "DFIDE.hpp"
#ifdef BIAL_DEBUG
#include "FileImage.hpp"
#endif

/* Implementation ***************************************************************************************************** */

namespace Bial {

  template< class D >
  GeodesicDistancePathFunction< D >::GeodesicDistancePathFunction( const Image< D > &handicap ) try : 
    PathFunction< Image, D >( ), handicap( handicap ) {
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
  GeodesicDistancePathFunction< D >::GeodesicDistancePathFunction( const GeodesicDistancePathFunction< D > &pf ) try :
    GeodesicDistancePathFunction< D >( pf.handicap ) {
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

  template< class D > GeodesicDistancePathFunction< D >::~GeodesicDistancePathFunction( ) {
  }

  template< class D > GeodesicDistancePathFunction< D >
  GeodesicDistancePathFunction< D >::operator=( const GeodesicDistancePathFunction< D > &pf ) {
    try {
      return( GeodesicDistancePathFunction< D >( pf ) );
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
  void GeodesicDistancePathFunction< D >::Initialize( Image< D > &init_value, Image< int > *init_label,
                                                      Image< int > *init_predecessor, bool sequential_label ) {
    try {
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
  bool GeodesicDistancePathFunction< D >::RemoveSimple( size_t, BucketState ) {
    return( true );
  }

  template< class D >
  bool GeodesicDistancePathFunction< D >::RemoveLabel( size_t index, BucketState state ) {
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
  inline bool GeodesicDistancePathFunction< D >::Capable( int index, int adj_index, BucketState adj_state ) {
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
  bool GeodesicDistancePathFunction< D >::Propagate( int index, int adj_index ) {
    try {
      D src_value = this->value->operator()( adj_index );
      COMMENT( "Computing spacial distance.", 3 );
      double distance = DFIDE::Distance( handicap.Coordinates( index ), handicap.Coordinates( adj_index ), 0, 0,
                                         handicap.Dims( ) );
      COMMENT( "Propagated value.", 3 );
      D prp_value = static_cast< D >( this->value->operator()( index ) + distance );
      COMMENT( "Updating value.", 3 );
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
  bool GeodesicDistancePathFunction< D >::Increasing( ) {
    return( true );
  }

#ifdef BIAL_EXPLICIT_GeodesicPathFunction

  template class GeodesicDistancePathFunction< int >;
  template class GeodesicDistancePathFunction< llint >;
  template class GeodesicDistancePathFunction< float >;
  template class GeodesicDistancePathFunction< double >;

#endif

}

#endif

#endif
