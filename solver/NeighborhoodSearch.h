#pragma once

#include "BasicOperations.h"

namespace SOLVER
{

   enum OBJECTFUNCTION
   {
      TIMETRAVELLED,
      DISTANCETRAVELLED
   };

   class NeighborhoodSearch : public BasicOperations
   {
   public:
      // We do copy the categorizedDeliveries to have the possibility to run different Optimizers in parallel if needed. 
      NeighborhoodSearch(const std::vector<BASE::DeliveryPoint> deliveries, int capacity, int numberOfVehicles,  int numberOfPickups, OBJECTFUNCTION objectvie);
      ~NeighborhoodSearch() = default;

      virtual auto execute() ->std::pair<std::vector<BASE::DeliveryPoint>, int> override;

   private:
      //! used to categorize the received delivery based on vehicle. 
      void fillCategorizedDeliveries();
      //! A neighborhood search which tries to find the nearest vehicle to shift the task if that reduces the total delivery time.
      void shiftTaskToNearestNeighbor3D();
      OBJECTFUNCTION m_objectvie;
   };
}
