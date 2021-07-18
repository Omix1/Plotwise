#pragma once

#include <vector>

namespace BASE
{
   class DeliveryPoint;
}

namespace SOLVER
{
   class BasicOperations
   {
   public:
      BasicOperations() = default;
      BasicOperations(std::vector<BASE::DeliveryPoint> deliveries, int capacity, int numberOfVehicles, int numberOfPickups);
      ~BasicOperations() = default;

      //! Should be implemented by inherited to solve the problem.
      virtual auto execute()->std::pair<std::vector<BASE::DeliveryPoint>, int> = 0;

   protected:
      //! Sort the deliveries based on their distance to Depot. Also considers the time as a dimension.
      void sortDeliveryiesBasedOnDistanceToDepot3D(std::vector<BASE::DeliveryPoint>& deliveries, const BASE::DeliveryPoint& depot);
      //! Checks if a delivery task for a vehicle consist of a pick up task.
      bool hasPickUpPoint(const std::vector<BASE::DeliveryPoint*>& deliveryTask);
      //! Returns the earliest on assigned task. 
      BASE::DeliveryPoint* isUnassignedDeliveryExist(std::vector<BASE::DeliveryPoint>& deliveries);
      //! Finds the nearest vehicle root (checks head position) and avoids the tabu candidates.
      BASE::DeliveryPoint* findNearestNeighbor3D(BASE::DeliveryPoint* delivery, std::vector<std::vector<BASE::DeliveryPoint*>>& categorizedDeliveries, std::vector<BASE::DeliveryPoint*>& tabuCandids);
      //! Finds the nearest delivery task from other delivery root.
      BASE::DeliveryPoint* findNearestNeighbor3D(BASE::DeliveryPoint* delivery, std::vector<BASE::DeliveryPoint> deliveries);
      //! Tries to assign a new task to a vehicle in the best order. 
      bool assignDeliveryToVehicle(BASE::DeliveryPoint* delivery, std::vector<BASE::DeliveryPoint*>& deliveryTask, int candidGroup);
      //! Checks if the delivery task for a vehicle is feasible.
      bool isTaskFeasible(std::vector<BASE::DeliveryPoint*>& deliveryTask);
      //! Calculates time traveled by a vehicle for delivery tasks assigned to it.
      int calculateTimeTraveled(std::vector<BASE::DeliveryPoint*>& deliveryTask);
      //! Calculates distance traveled by all vehicles for delivery tasks assigned to them.
      int calculateTotalTimeTraveled(std::vector<std::vector<BASE::DeliveryPoint*>>& categorizedDeliveries);
      //! Calculates distance traveled by a vehicle for delivery tasks assigned to it.
      int calculateDistanceTraveled(std::vector<BASE::DeliveryPoint*>& deliveryTask);      

      std::vector<BASE::DeliveryPoint> m_deliveries;
      std::vector<std::vector<BASE::DeliveryPoint*>> m_categorizedDeliveries;
      BASE::DeliveryPoint* m_depot;
      int m_numberOfVehicles = 0;
      int m_numberOfPickups = 0;
      int m_capacity = 0;
   };
}