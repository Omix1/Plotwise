#include "HeuristicTSP.h"
#include "../datareader/io_datareader.h"
#include "helper.h"

#include <algorithm>
#include <utility>

#include "NeighborhoodSearch.h"

SOLVER::HeuristicTSP::HeuristicTSP(std::vector<BASE::DeliveryPoint> deliveries, int capacity, int numberOfVehicles, int numberOfPickups)
   : BasicOperations(deliveries, capacity, numberOfVehicles, numberOfPickups)
   , m_copyOfDeliveries{deliveries}
{
}

auto SOLVER::HeuristicTSP::execute() ->std::pair<std::vector<BASE::DeliveryPoint>, int>
{
   m_numberOfVehicles = 0; //We overnight the number of vehicle read from benchmark as we want to work minimum vehicles.

   // If we could not solve the problem, we add 1 more vehicle and try reschedule.
   while (!m_problemSolved)
   {
      m_numberOfVehicles++;
      m_problemSolved = true;
      m_categorizedDeliveries.clear();
      m_deliveries = m_copyOfDeliveries;
      // Number of vehicles should be equal or bigger than number of pickups tasks.   
      m_numberOfVehicles = std::max(m_numberOfVehicles, m_numberOfPickups);
      distributeDeliveries();
   }   

   return std::make_pair(m_deliveries, m_numberOfVehicles);
}

void SOLVER::HeuristicTSP::distributeDeliveries()
{
   sortDeliveryiesBasedOnDistanceToDepot3D(m_deliveries, *m_depot);

   // Add first delivery assignment to each vehicle.
   for (int i = 1/*Skip depot*/; i <= m_numberOfVehicles; i++) {
      m_deliveries[i].setCategoryNumber(i);
      m_categorizedDeliveries.push_back(std::vector<BASE::DeliveryPoint*> {&m_deliveries[i]});
   }

   // Assigning the remaining deliveries.
   std::vector<BASE::DeliveryPoint*> tabuCandids;
   while (auto* delivery = isUnassignedDeliveryExist(m_deliveries))
   {
      if (auto* candid = findNearestNeighbor3D(delivery, m_categorizedDeliveries, tabuCandids))
      {
         auto succed = assignDeliveryToVehicle(delivery, m_categorizedDeliveries[candid->getCategoryNumber() - 1/*index starts from zero*/], candid->getCategoryNumber());
         if (!succed)
         {
            // Candid vehicle exist, but we are not able to assign this task, as the task gets unfeasible.
            // In this case we try to choose another vehicle.
            tabuCandids.push_back(candid);
            continue;
         }
      }
      else
      {
         // we can not assign this delivery to any vehicle.
         //delivery->setCategoryNumber(-2);
         m_problemSolved = false;
         break;
      }
      // If we are here, the task is assigned or windrowed. 
      tabuCandids.clear();
   }
}
