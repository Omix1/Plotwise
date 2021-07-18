#pragma once


#include "../datastructure/DeliveryPoint.h"

#include <string>
#include <vector>

namespace SOLVER
{
   class TSPManager
   {
   public:
      TSPManager(std::string fileName);
      ~TSPManager() = default;      

      //! solving the problem.
      void runSolvers();      

   private:
      //! reads the data about vehicles from benchmark file.
      bool getVehiclesData();
      //! reads data about the deliveries.
      bool getDeliveryData();
      //! this method will mark the first point as Depot and changes some points as PickUp.
      bool markAsDepotAndPickUpPoint();
      //! used to categorize the received delivery based on vehicle. 
      std::vector<std::vector<BASE::DeliveryPoint*>> fillCategorizedDeliveries();

      std::string m_fileName;
      std::vector<BASE::DeliveryPoint> m_deliveries;
      std::vector<std::vector<BASE::DeliveryPoint*>> m_categorizedDeliveries;
      BASE::DeliveryPoint* m_depot;
      int m_numberOfVehicles = 0;
      int m_numberOfPickups = 0;
      int m_capacity = 0;
   };
}