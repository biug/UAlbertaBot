#pragma once

#include "Common.h"

#include "MetaType.h"

namespace CasiaBot
{
struct BuildOrderItem
{
    MetaType			metaType;		// the thing we want to 'build'
    int					priority;	// the priority at which to place it in the queue
    bool				blocking;	// whether or not we block further items
    bool                isGasSteal;

    BuildOrderItem(MetaType m,int p,bool b,bool gasSteal = false)
        : metaType(m)
        , priority(p)
        , blocking(b)
        , isGasSteal(gasSteal) 
    {
    }

    bool operator<(const BuildOrderItem &x) const
    {
        return priority < x.priority;
    }
};

class BuildOrderQueue
{
    std::deque< BuildOrderItem >			queue;

    int lowestPriority;
    int highestPriority;
    int defaultPrioritySpacing;

    int numSkippedItems;

public:

    BuildOrderQueue();

    void clearAll();											// clears the entire build order queue
    void skipItem();											// increments skippedItems
    void queueAsHighestPriority(MetaType m,bool blocking,bool gasSteal = false);		// queues something at the highest priority
    void queueAsLowestPriority(MetaType m,bool blocking);		// queues something at the lowest priority
    void queueItem(BuildOrderItem b);			// queues something with a given priority
    void removeHighestPriorityItem();								// removes the highest priority item
    void removeCurrentHighestPriorityItem();

    int getHighestPriorityValue();								// returns the highest priority value
    int	getLowestPriorityValue();								// returns the lowest priority value
    size_t size();													// returns the size of the queue

    bool isEmpty();

    void removeAll(MetaType m);									// removes all matching meta types from queue

    BuildOrderItem & getHighestPriorityItem();	// returns the highest priority item
    BuildOrderItem & getNextHighestPriorityItem();	// returns the highest priority item

    bool canSkipItem();
    bool hasNextHighestPriorityItem();								// returns the highest priority item

    void drawQueueInformation(int x,int y);

    // overload the bracket operator for ease of use
    BuildOrderItem operator [] (int i);
};
}