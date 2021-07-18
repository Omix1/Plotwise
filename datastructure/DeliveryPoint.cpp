#include "DeliveryPoint.h"

BASE::DeliveryPoint::DeliveryPoint(int custNo, int xCoord, int yCoord, int demand, int readyTime, int dueDate, int serviceTime, DeliveryType type)
   : m_custNo{ custNo }
   , m_xCoord{ xCoord }
   , m_yCoord{ yCoord }
   , m_demand{ demand }
   , m_readyTime{ readyTime }
   , m_dueDate{ dueDate }
   , m_serviceTime{ serviceTime }
   , m_type{type}
{
}