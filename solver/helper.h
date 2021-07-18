#pragma once

#include "../datastructure/DeliveryPoint.h"

#include <vector>
#include <iostream>
#include <iomanip>
#include <iosfwd>
#include <string>
#include <limits.h>
#include <stdio.h>



namespace HELPER
{
   // Calculates the distance between two points (2D). For simplicity I am rounding the the distances to integer.
   static int distance(int x1, int y1, int x2, int y2)
   {
      return round(sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)));
   }


   // Calculates the distance between two points (3D). For simplicity I am rounding the the distances to integer.
   // We consider the ReadyTime as a dimension.
   static int distance(int x1, int y1, int z1, int x2, int y2, int z2)
   {
      return round(sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2)));
   }


   // Calculates the distance between two points (2D). For simplicity I am rounding the the distances to integer.
   static int distance2D(const BASE::DeliveryPoint& a, const BASE::DeliveryPoint& b)
   {
      return  HELPER::distance(a.getXCoord(), a.getYCoord(), b.getXCoord(), b.getYCoord());
   }


   // Calculates the distance between two points (3D). For simplicity I am rounding the the distances to integer.
   // We consider the ReadyTime as a dimension.
   static int distance3D(const BASE::DeliveryPoint& a, const BASE::DeliveryPoint& b)
   {
      return  HELPER::distance(a.getXCoord(), a.getYCoord(), a.getZCoord(), b.getXCoord(), b.getYCoord(), b.getZCoord());
   }

   static std::string convertToTime(int num)
   {
      // lets keep it simple for this problem.
      int hour = num / 60;
      int minute = num % 60;
      char buffer[10];

      sprintf(buffer, "%02d:%02d", hour, minute);
      return  buffer;
   }


   // Used to print the final solution.
   static void printPlanning(const std::vector<std::vector<BASE::DeliveryPoint*>>& categorizedDeliveries, const BASE::DeliveryPoint* depot)
   {
      std::cout << "Do you want to print the deliveries (Y), or only see the final statistics(N):";
      char ch;
      std::cin >> ch;

      int totalTime = 0;
      int numberOfVehicles = 0;
      int totalDistance = 0;
      int allVehicleintTotalDistance = 0;
      for (auto& deliveryTask : categorizedDeliveries)
      {
         if (deliveryTask.empty())
            continue;
         numberOfVehicles++;
         auto* lastDeliveryPoint = depot; // Each time starts from depot.
         int totoalTime = depot->getReadyTime() + depot->getServiceTime();
         totalTime += totoalTime;
         if (ch == 'Y' || ch == 'y')
         {
            std::cout << "     CUST NO.   XCOORD.   YCOORD.  DEMAND  READYTIME  DUEDATE  SERVICETIME  PLANEDARRIVAL  PLANEDEPARTEURE  DISTANCE  PICKUP" << std::endl;
            std::cout << "     Depot" << std::setw(10) << depot->getXCoord() << std::setw(10)
               << depot->getYCoord() << std::setw(10) << depot->getDemand() << std::setw(10)
               << HELPER::convertToTime(depot->getReadyTime()) << std::setw(10) << HELPER::convertToTime(depot->getDueDate())
               << std::setw(10) << HELPER::convertToTime(depot->getServiceTime())
               << std::setw(15) << HELPER::convertToTime(totoalTime)
               << std::setw(15) << HELPER::convertToTime(totoalTime)
               << std::setw(12) << totalDistance << "km"
               << std::setw(8) << (depot->getType() == BASE::DeliveryType::PICKUP) << std::endl;
         }
         // Visit all delivery tasks.
         for (int i = 0; i < deliveryTask.size(); i++)
         {
            auto* nextDeliveryPoint = deliveryTask[i];
            int distance = HELPER::distance(lastDeliveryPoint->getXCoord(), lastDeliveryPoint->getYCoord(),
               nextDeliveryPoint->getXCoord(), nextDeliveryPoint->getYCoord());
            totalDistance += distance;
            lastDeliveryPoint = nextDeliveryPoint;

            // Maybe we reach earlier than ReadyTime, so we should wait. 
            auto arrival = std::max(totoalTime + distance, lastDeliveryPoint->getReadyTime());
            totoalTime = arrival + lastDeliveryPoint->getServiceTime();
            if (ch == 'Y' || ch == 'y')
               std::cout << std::setw(10) << nextDeliveryPoint->getCustNo() << std::setw(10) << nextDeliveryPoint->getXCoord() << std::setw(10)
               << nextDeliveryPoint->getYCoord() << std::setw(10) << nextDeliveryPoint->getDemand() << std::setw(10)
               << HELPER::convertToTime(nextDeliveryPoint->getReadyTime()) << std::setw(10) << HELPER::convertToTime(nextDeliveryPoint->getDueDate())
               << std::setw(10) << HELPER::convertToTime(nextDeliveryPoint->getServiceTime())
               << std::setw(15) << HELPER::convertToTime(arrival)
               << std::setw(15) << HELPER::convertToTime(totoalTime)
               << std::setw(12) << totalDistance << "km"
               << std::setw(8) << (nextDeliveryPoint->getType() == BASE::DeliveryType::PICKUP) << std::endl;
         }

         //Return to depot
         const BASE::DeliveryPoint* nextDeliveryPoint = depot;
         int distance = HELPER::distance(lastDeliveryPoint->getXCoord(), lastDeliveryPoint->getYCoord(),
            nextDeliveryPoint->getXCoord(), nextDeliveryPoint->getYCoord());
         totalDistance += distance;
         lastDeliveryPoint = nextDeliveryPoint;

         // Maybe we reach earlier than ReadyTime, so we should wait. 
         auto arrival = std::max(totoalTime + distance, lastDeliveryPoint->getReadyTime());
         totoalTime = arrival + lastDeliveryPoint->getServiceTime();
         totalTime += totoalTime;
         if (ch == 'Y' || ch == 'y')
         {
            std::cout << "     Depot" << std::setw(10) << nextDeliveryPoint->getXCoord()
               << std::setw(10) << nextDeliveryPoint->getYCoord()
               << "       ---  " << "    ---  " << "     --- " << "      --- "
               << std::setw(15) << HELPER::convertToTime(arrival)
               << "           --- "
               << std::setw(12) << totalDistance << "km"
               << std::setw(8) << (nextDeliveryPoint->getType() == BASE::DeliveryType::PICKUP) << std::endl;
            std::cout << std::endl;
         }
         allVehicleintTotalDistance += totalDistance;
         totalDistance = 0;
      }

      std::cout << " Total Traveled Time: " << HELPER::convertToTime(totalTime) << " (hh:mm)" << std::endl <<
         " Total Distance Traveled: " << allVehicleintTotalDistance << " km" << std::endl <<
         " Number Of Used Vehicles: " << numberOfVehicles << std::endl;
   }
}

