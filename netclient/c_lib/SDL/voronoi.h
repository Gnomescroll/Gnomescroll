// voronoi.h
//
// Copyright (C) 2003, 2004 Jason Bevins
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig'.)
//

/* Modified by m3th 2011 */

#pragma once

#include "modulebase.h"

namespace noise
{

  namespace module
  {

    /// Default displacement to apply to each cell for the
    /// noise::module::Voronoi noise module.
    const double DEFAULT_VORONOI_DISPLACEMENT = 1.0;

    /// Default frequency of the seed points for the noise::module::Voronoi
    /// noise module.
    const double DEFAULT_VORONOI_FREQUENCY = 1.0;

    /// Default seed of the noise function for the noise::module::Voronoi
    /// noise module.
    const int DEFAULT_VORONOI_SEED = 0;

    class Voronoi: public Module
    {

      public:

        Voronoi ();

        void EnableDistance (bool enable = true)
        {
          m_enableDistance = enable;
        }

        double GetDisplacement () const
        {
          return m_displacement;
        }

        double GetFrequency () const
        {
          return m_frequency;
        }

        virtual int GetSourceModuleCount () const
        {
          return 0;
        }

        int GetSeed () const
        {
          return m_seed;
        }

        bool IsDistanceEnabled () const
        {
          return m_enableDistance;
        }

        virtual double GetValue (double x, double y, double z) const;

        void SetDisplacement (double displacement)
        {
          m_displacement = displacement;
        }

        void SetFrequency (double frequency)
        {
          m_frequency = frequency;
        }

        /// Sets the seed value used by the Voronoi cells
        ///
        /// @param seed The seed value.
        ///
        /// The positions of the seed values are calculated by a
        /// coherent-noise function.  By modifying the seed value, the output
        /// of that function changes.
        void SetSeed (int seed)
        {
          m_seed = seed;
        }

      protected:

        /// Scale of the random displacement to apply to each Voronoi cell.
        double m_displacement;

        /// Determines if the distance from the nearest seed point is applied to
        /// the output value.
        bool m_enableDistance;

        /// Frequency of the seed points.
        double m_frequency;

        /// Seed value used by the coherent-noise function to determine the
        /// positions of the seed points.
        int m_seed;

    };

    /// @}

    /// @}

    /// @}

  }

}

#endif
