#pragma once
#include <corecrt_math.h>

namespace BASE
{
   enum DeliveryType
   {
      DEPOT,
      DELIVERY,
      PICKUP
   };

   class DeliveryPoint
   {
   public:
      DeliveryPoint(int custNo, int xCoord, int yCoord, int demand, int readyTime, int dueDate, int serviceTime, DeliveryType type);
      ~DeliveryPoint() = default;

      int getCustNo() const { return m_custNo; }
      void setCustNo(int val) { m_custNo = val; }
      int getXCoord() const { return m_xCoord; }
      void setXCoord(int val) { m_xCoord = val; }
      int getYCoord() const { return m_yCoord; }
      void setYCoord(int val) { m_yCoord = val; }
      // We consider Time as a Z axis. We assume they have one to one relation. it means that 1 minute is km.
      int getZCoord() const { return m_readyTime; }
      int getDemand() const { return m_demand; }
      void setDemand(int val) { m_demand = val; }
      int getReadyTime() const { return m_readyTime; }
      void setReadyTime(int val) { m_readyTime = val; }
      int getDueDate() const { return m_dueDate; }
      void setDueDate(int val) { m_dueDate = val; }
      int getServiceTime() const { return m_serviceTime; }
      void setServiceTime(int val) { m_serviceTime = val; }
      int getCategoryNumber() const { return categoryNumber; }
      void setCategoryNumber(int val) { categoryNumber = val; }
      DeliveryType getType() const { return m_type; }
      void setType(DeliveryType type) { m_type = type; }
      
   private:
      int m_custNo;
      int m_xCoord;
      int m_yCoord;
      int m_demand;
      int m_readyTime;
      int m_dueDate;
      int m_serviceTime;
      int categoryNumber=-1;
      DeliveryType m_type;
   };
}