#include "GblTrajectory.h"
#include "materials.h"

namespace gblsim {
  class plane {
  public:
    plane();
    plane(double position, double material, bool measurement, double resolution = 0);

    double position() { return m_position; }

    bool operator < (const plane& pl) const {
        return (m_position < pl.m_position);
    }

  private:
    bool m_measurement;
    double m_resolution;
    bool m_scatterer;
    double m_materialbudget;
    double m_position;

    friend class telescope;

  };

  class scatterer : public plane {
  public:
  scatterer(double position, double material) : plane(position, material, false) {};
  };

  class reference : public plane {
  public:
  reference(double position) : plane(position, 0.0, false) {};
  };
    
  class telescope {
  public:
    telescope(std::vector<gblsim::plane> planes, double beam_energy, double material = X0_Air);

    // Return the trajectory
    gbl::GblTrajectory getTrajectory();

    // Return the resolution at given plane:
    double getResolution(int plane);

    void printLabels();
  private:
    // Radiationlength of the material of the surrounding volume, defaults to dry air:
    double m_volumeMaterial;
    
    double getTotalMaterialBudget(std::vector<gblsim::plane> planes);
    std::vector<gbl::GblPoint> m_listOfPoints;
    std::vector<int> m_listOfLabels;
  };
}
