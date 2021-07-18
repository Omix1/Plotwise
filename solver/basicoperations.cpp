#include "basicoperations.h"

#include "helper.h"

#include <algorithm>
#include <cassert>

SOLVER::BasicOperations::BasicOperations(std::vector<BASE::DeliveryPoint> deliveries, int capacity, int numberOfVehicles, int numberOfPickups)
   : m_deliveries{ deliveries }
   , m_capacity{ capacity }
   , m_numberOfVehicles{ numberOfVehicles }
   , m_numberOfPickups{numberOfPickups}
{
   m_depot = &m_deliveries[0];
}

void SOLVER::BasicOperations::sortDeliveryiesBasedOnDistanceToDepot3D(std::vector<BASE::DeliveryPoint>& deliveries, const BASE::DeliveryPoint& depot)
{
   std::sort(deliveries.begin() + 1 /*No sort for depot*/, deliveries.end(), [&depot](auto a, auto b) { return HELPER::distance3D(depot, a) < HELPER::distance3D(depot, b); });
}

bool SOLVER::BasicOperations::hasPickUpPoint(const std::vector<BASE::DeliveryPoint*>& deliveryTask)
{
   for (auto* delivery : deliveryTask)
      if (delivery->getType() == BASE::DeliveryType::PICKUP)
         return true;

   return false;
}

BASE::DeliveryPoint* SOLVER::BasicOperations::isUnassignedDeliveryExist(std::vector<BASE::DeliveryPoint>& deliveries)
{
   // The first one considered as Depot.
   if (deliveries.size() <= 1)
      return nullptr;

   for (auto it = deliveries.begin() + 1/*Skip the depot*/; it != deliveries.end(); it++)
      if ((*it).getCategoryNumber() == -1)
         return &(*it);

   return nullptr;
}

BASE::DeliveryPoint* SOLVER::BasicOperations::findNearestNeighbor3D(BASE::DeliveryPoint* delivery, std::vector<std::vector<BASE::DeliveryPoint*>>& categorizedDeliveries, std::vector<BASE::DeliveryPoint*>& tabuCandids)
{
   BASE::DeliveryPoint* selectedCandid = nullptr;
   int minDistance = INT_MAX;
   for (auto deliveries : categorizedDeliveries)
   {
      if (!deliveries.empty())
      {
         // finds the nearest point which is not in the tabu list. 
         auto* candidDelivery = deliveries[deliveries.size() - 1];
         int distance = HELPER::distance3D(*delivery, *candidDelivery);
         if (distance < minDistance && std::find(tabuCandids.begin(), tabuCandids.end(), candidDelivery) == tabuCandids.end())
         {
            minDistance = distance;
            selectedCandid = candidDelivery;
         }
      }
   }
   return selectedCandid;
}

BASE::DeliveryPoint* SOLVER::BasicOperations::findNearestNeighbor3D(BASE::DeliveryPoint* task, std::vector<BASE::DeliveryPoint> deliveries)
{
   BASE::DeliveryPoint* selectedCandid = nullptr;
   int minDistance = INT_MAX;
   for (auto& delivery : m_deliveries)
      if (task->getCustNo() != delivery.getCustNo() && task->getCategoryNumber() != delivery.getCategoryNumber())
      {
         int distance = HELPER::distance3D(*task, delivery);
         if (distance < minDistance)
         {
            minDistance = distance;
            selectedCandid = &delivery;
         }
      }
   return selectedCandid;

}

bool SOLVER::BasicOperations::assignDeliveryToVehicle(BASE::DeliveryPoint* delivery, std::vector<BASE::DeliveryPoint*>& deliveryTask, int candidGroup)
{
   // we cannot add a new pickup task to a vehicle that already has Pick up.
   if (delivery->getType() == BASE::DeliveryType::PICKUP && hasPickUpPoint(deliveryTask))
      return false;

   // Search for the best position to assign the task. 
   int minDistance = INT_MAX;
   int minPosition = -1;
   for (int position = 0; position <= deliveryTask.size(); position++)
   {
      //Adding the delivery to task into place number i;
      deliveryTask.insert(deliveryTask.begin() + position, delivery);
      // checking the new task feasibility
      if (isTaskFeasible(deliveryTask))
      {
         // check if it is minimum.
         auto distanceTraveled = calculateTimeTraveled(deliveryTask);
         if (distanceTraveled < minDistance)
         {
            minDistance = distanceTraveled;
            minPosition = position;
         }
      }
      // remove the task from temporary position.
      deliveryTask.erase(deliveryTask.begin() + position);
   }

   //Adding the delivery to task into place number i as fixed place;
   if (minPosition != -1)
   {
      delivery->setCategoryNumber(candidGroup);
      deliveryTask.insert(deliveryTask.begin() + minPosition, delivery);
      return true;
   }

   return false;
}

//! Checks if the delivery task for a vehicle is feasible.
bool SOLVER::BasicOperations::isTaskFeasible(std::vector<BASE::DeliveryPoint*>& deliveryTask)
{
   if (deliveryTask.empty())
      return true;

   /////////////////////////////////////////
   // This part checks the time feasibility.
   auto* lastDeliveryPoint = m_depot;
   int totoalTime = m_depot->getReadyTime() + m_depot->getServiceTime();
   for (int i = 0; i < deliveryTask.size(); i++)
   {
      auto* nextDeliveryPoint = deliveryTask[i];
      int distance = HELPER::distance(lastDeliveryPoint->getXCoord(), lastDeliveryPoint->getYCoord(),
         nextDeliveryPoint->getXCoord(), nextDeliveryPoint->getYCoord());

      //Due date passed, we cannot handle this mission.
      if (totoalTime + distance > nextDeliveryPoint->getDueDate())
         return false;

      lastDeliveryPoint = nextDeliveryPoint;
      // Maybe we reach earlier than ReadyTime, so we should wait. 
      totoalTime = std::max(totoalTime + distance, lastDeliveryPoint->getReadyTime()) + lastDeliveryPoint->getServiceTime();
   }

   //Return back to depot
   int distance = HELPER::distance(lastDeliveryPoint->getXCoord(), lastDeliveryPoint->getYCoord(),
      m_depot->getXCoord(), m_depot->getYCoord());
   //Due date passed, we cannot return depot.
   if (totoalTime + distance > m_depot->getDueDate())
      return false;

   ///////////////////////////////////
   // checks the capacity feasibility.
   int pickupIndex = -1;
   int demands = 0;
   // Compute capacity needed for all demands. 
   for (int i = 0; i < deliveryTask.size(); i++)
   {
      if (deliveryTask[i]->getType() == BASE::DeliveryType::PICKUP)
         pickupIndex = i;
      else
         demands += deliveryTask[i]->getDemand();
   }
   if (demands > m_capacity)
      return false;

   // Compute capacity needed after the pickup point.
   if (pickupIndex != -1) // if there is pickup task.
   {
      demands = 0;
      for (int i = pickupIndex + 1; i < deliveryTask.size(); i++)
      {
         demands += deliveryTask[i]->getDemand();
      }
      // A part of available capacity used by pickup.
      if (demands > m_capacity - deliveryTask[pickupIndex]->getDemand())
         return false;
   }
   return true;
}

//! Calculates distance traveled by a vehicle for delivery tasks assigned to it.
int SOLVER::BasicOperations::calculateTimeTraveled(std::vector<BASE::DeliveryPoint*>& deliveryTask)
{
   assert(isTaskFeasible(deliveryTask));
   int totalTime = m_depot->getReadyTime();
   const BASE::DeliveryPoint* lastDeliveryPoint = m_depot;
   for (auto* nextDeliveryPoint : deliveryTask)
   {
      int distance = HELPER::distance(lastDeliveryPoint->getXCoord(), lastDeliveryPoint->getYCoord(),
         nextDeliveryPoint->getXCoord(), nextDeliveryPoint->getYCoord());

      lastDeliveryPoint = nextDeliveryPoint;
      // Maybe the vehicle arrives earlier than the ReadyTime, so it should wait. 
      totalTime = std::max(totalTime + distance, lastDeliveryPoint->getReadyTime()) + lastDeliveryPoint->getServiceTime();
   }

   // Return back to Depot
   int distance = HELPER::distance(lastDeliveryPoint->getXCoord(), lastDeliveryPoint->getYCoord(),
      m_depot->getXCoord(), m_depot->getYCoord());   
   totalTime = totalTime + distance;
   return totalTime;
}

int SOLVER::BasicOperations::calculateDistanceTraveled(std::vector<BASE::DeliveryPoint*>& deliveryTask)
{
   assert(isTaskFeasible(deliveryTask));
   int totalDistance = m_depot->getReadyTime();
   const BASE::DeliveryPoint* lastDeliveryPoint = m_depot;
   for (auto* nextDeliveryPoint : deliveryTask)
   {
      int distance = HELPER::distance(lastDeliveryPoint->getXCoord(), lastDeliveryPoint->getYCoord(),
         nextDeliveryPoint->getXCoord(), nextDeliveryPoint->getYCoord());
      totalDistance += distance;
      lastDeliveryPoint = nextDeliveryPoint;
   }

   // Return back to Depot
   int distance = HELPER::distance(lastDeliveryPoint->getXCoord(), lastDeliveryPoint->getYCoord(),
      m_depot->getXCoord(), m_depot->getYCoord());
   totalDistance += distance;
   return totalDistance;
}

//! Calculates distance traveled by all vehicles for delivery tasks assigned to them.
int SOLVER::BasicOperations::calculateTotalTimeTraveled(std::vector<std::vector<BASE::DeliveryPoint*>>& categorizedDeliveries)
{
   int totalDistanceTraveled = 0;
   for (auto& deliveryTask : categorizedDeliveries)
   {      
      totalDistanceTraveled += calculateTimeTraveled(deliveryTask);
   }
   return totalDistanceTraveled;
}

