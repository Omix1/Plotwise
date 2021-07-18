#include "NeighborhoodSearch.h"

#include "../datastructure/DeliveryPoint.h"
#include <algorithm>
#include <iostream>

using namespace SOLVER;

NeighborhoodSearch::NeighborhoodSearch(std::vector<BASE::DeliveryPoint> deliveries, int capacity, int numberOfVehicles, int numberOfPickups, OBJECTFUNCTION objectvie)
   : BasicOperations(deliveries, capacity, numberOfVehicles, numberOfPickups)
   , m_objectvie{objectvie}
{
   fillCategorizedDeliveries();
}


auto SOLVER::NeighborhoodSearch::execute() ->std::pair<std::vector<BASE::DeliveryPoint>, int>
{
   shiftTaskToNearestNeighbor3D();
   return std::make_pair(m_deliveries, m_numberOfVehicles);
}

void SOLVER::NeighborhoodSearch::fillCategorizedDeliveries()
{
   // Transforming the results to categorized deliveries for reporting purpose. 
   m_categorizedDeliveries.clear();
   for (int i = 0; i < m_numberOfVehicles; i++)
   {
      std::vector<BASE::DeliveryPoint*> categorizedDeliveries{};
      m_categorizedDeliveries.push_back(categorizedDeliveries);
   }

   for (int i = 1; i < m_deliveries.size(); i++)
   {
      auto& deliveryTask = m_deliveries[i];
      auto& deliveryJob = m_categorizedDeliveries[deliveryTask.getCategoryNumber() - 1];
      deliveryJob.push_back(&deliveryTask);      
   }
}

void NeighborhoodSearch::shiftTaskToNearestNeighbor3D()
{
   bool changed = false;
   do
   {
      changed = false;
      // We try to shift one task each time to other DeliveryRoutes each time with the hope to find a shorter path.
      // We continue until we don't find a shorter route for any of the tasks.
      for (int i = 0; i < m_categorizedDeliveries.size(); i++)
      {
         for (int j = 0; j < m_categorizedDeliveries.size(); j++)
         {
            if (i != j)
            {
               int k = 0;
               while ( k < m_categorizedDeliveries[i].size())
               {
                  // We copy the delivery task, if shift was not successful no need to undo.
                  auto currentTask = m_categorizedDeliveries[i];
                  auto checkingTask = m_categorizedDeliveries[j];
                  if (!currentTask.empty() && !checkingTask.empty())
                  {
                     int costOld = 0;
                     if (m_objectvie == OBJECTFUNCTION::DISTANCETRAVELLED)
                        costOld = calculateDistanceTraveled(currentTask) + calculateDistanceTraveled(checkingTask);
                     else
                        costOld = calculateTimeTraveled(currentTask) + calculateTimeTraveled(checkingTask);
                     auto candid = currentTask[k];
                     // Keep the old category for undo case.
                     auto oldCategoryNumber = candid->getCategoryNumber();
                     // Try to assign it another vehicle. 
                     auto succed = assignDeliveryToVehicle(candid, checkingTask, checkingTask[0]->getCategoryNumber());
                     if (succed)
                     {
                        currentTask.erase(currentTask.begin() + k);
                        // Summation of distance or time for both delivery jobs should reduce. Then we accept the solution.
                        int costNew;
                        if (m_objectvie == OBJECTFUNCTION::DISTANCETRAVELLED)
                           costNew = calculateDistanceTraveled(currentTask) + calculateDistanceTraveled(checkingTask);
                        else
                           costNew = calculateTimeTraveled(currentTask) + calculateTimeTraveled(checkingTask);
                        if (costNew < costOld)
                        {
                           changed = true;
                           m_categorizedDeliveries[i] = currentTask;
                           m_categorizedDeliveries[j] = checkingTask;                           
                        }
                        else
                        {
                           // Return the task back to old delivery task. 
                           candid->setCategoryNumber(oldCategoryNumber);
                        }
                     }
                  }
                  k++;
               }
            }
         }
      }
   } while (changed);   
}