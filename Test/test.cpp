#include "pch.h"
#include "../datastructure/DeliveryPoint.h"
#include "../datareader/io_datareader.h"
#include "../solver/basicoperations.h"
using namespace BASE;

namespace PlotWiseTests
{
   TEST(PlotWise, DeliveryPoint) 
   {
      DeliveryPoint a(4, 10, 10, 100, 616, 661, 90, DeliveryType::DELIVERY);
      EXPECT_EQ(a.getXCoord(), 10);
      EXPECT_EQ(a.getType(), BASE::DeliveryType::DELIVERY);
      EXPECT_EQ(a.getCategoryNumber(), -1);
      a.setCategoryNumber(4);
      EXPECT_EQ(a.getCategoryNumber(), 4);
      DeliveryPoint b(5, 25, 26, 20, 128, 179, 90, DeliveryType::DEPOT);
      EXPECT_EQ(b.getType(), BASE::DeliveryType::DEPOT);
      EXPECT_EQ(b.getZCoord(), b.getReadyTime());
   }

   TEST(PlotWise, IOHandler) 
   {
      int  numberOfVehicles, capacity;
      std::string filename = "../../benchmark/C1_2_1.TXT";
      IO::IOHandler::readVehiclesData(filename, numberOfVehicles, capacity);
      EXPECT_EQ(capacity, 150);
      EXPECT_EQ(numberOfVehicles, 1);
      std::vector<DeliveryPoint> deliveries;
      IO::IOHandler::readDeliveryData(filename, deliveries);
      EXPECT_EQ(deliveries.size(), 201);
   }


   TEST(PlotWise, BasicOperations)
   {
      class Test : public SOLVER::BasicOperations
      {
      public:
         Test(std::vector<BASE::DeliveryPoint> deliveries, int capacity, int numberOfVehicles, int numberOfPickups)
            // The BasicOperations makes a copy of objects, so we cannot test it using pointer. 
            : BasicOperations(deliveries, capacity, numberOfVehicles, numberOfPickups)
         {}

         virtual auto execute()->std::pair<std::vector<BASE::DeliveryPoint>, int> override
         { return std::make_pair(std::vector<BASE::DeliveryPoint>{}, 0); };
         bool isTaskFeasibleTest(std::vector<BASE::DeliveryPoint*>& deliveryTask)
         {
            return isTaskFeasible(deliveryTask);
         }
         int calculateTimeTraveledTest(std::vector<BASE::DeliveryPoint*>& deliveryTask) {
            return calculateTimeTraveled(deliveryTask);
         }
         int calculateDistanceTraveledTest(std::vector<BASE::DeliveryPoint*>& deliveryTask)
         {
            return calculateDistanceTraveled(deliveryTask);
         }
         bool hasPickUpPointTest(const std::vector<BASE::DeliveryPoint*>& deliveryTask)
         {
            return hasPickUpPoint(deliveryTask);
         }
         BASE::DeliveryPoint* isUnassignedDeliveryExistTest(std::vector<BASE::DeliveryPoint>& deliveries)
         {
            return isUnassignedDeliveryExist(deliveries);
         }

         ~Test() = default;
      private:
      };

      int  numberOfVehicles=1, capacity =100;
      std::vector<DeliveryPoint> deliveries;
      std::vector<DeliveryPoint*> deliveriesTask;

      DeliveryPoint a(4, 10, 10, 50, 0, 1000, 90, DeliveryType::DEPOT);
      DeliveryPoint b(5, 25, 26, 20, 128, 179, 90, DeliveryType::PICKUP);
      DeliveryPoint c(6, 25, 26, 20, 500, 600, 90, DeliveryType::DELIVERY);
      DeliveryPoint d(7, 25, 26, 20, 128, 179, 90, DeliveryType::DELIVERY);
      
      deliveries.push_back(a);
      deliveries.push_back(b);
      deliveries.push_back(c);
      deliveries.push_back(d);     

      //Checking the object categorization.
      Test test(deliveries, capacity, numberOfVehicles, 1);
      auto *delivery = test.isUnassignedDeliveryExistTest(deliveries);
      EXPECT_EQ(delivery->getCustNo(), b.getCustNo());
      delivery->setCategoryNumber(50);
      delivery = test.isUnassignedDeliveryExistTest(deliveries);
      EXPECT_EQ(delivery->getCustNo(), c.getCustNo());
      delivery->setCategoryNumber(60);
      delivery = test.isUnassignedDeliveryExistTest(deliveries);
      EXPECT_EQ(delivery->getCustNo(), d.getCustNo());
      delivery->setCategoryNumber(70);
      delivery = test.isUnassignedDeliveryExistTest(deliveries);
      EXPECT_EQ(delivery, nullptr);

      // Check feasibility of a delivery task.
      deliveriesTask.push_back(&b);
      deliveriesTask.push_back(&c);
      ASSERT_TRUE(test.isTaskFeasibleTest(deliveriesTask));
      ASSERT_TRUE(test.hasPickUpPointTest(deliveriesTask));
      // Check the time and distance traveled by a delivery task.
      EXPECT_EQ(test.calculateTimeTraveledTest(deliveriesTask), 612);
      EXPECT_EQ(test.calculateDistanceTraveledTest(deliveriesTask), 44);

      deliveriesTask.clear();      
      deliveriesTask.push_back(&c);
      deliveriesTask.push_back(&b);      
      ASSERT_FALSE(test.isTaskFeasibleTest(deliveriesTask));

      deliveriesTask.clear();
      deliveriesTask.push_back(&c);
      deliveriesTask.push_back(&d);
      ASSERT_FALSE(test.hasPickUpPointTest(deliveriesTask));

      // Sure, I can write more challenging unit tests but I am stopping here due to lack of time.
   }
}