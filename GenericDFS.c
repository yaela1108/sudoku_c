// -------------------------- includes -------------------------

#include "GenericDFS.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

// -------------------------- functions -------------------------

/**
 * See documentation for getBest().
 */
static pNode getSubtreeBest(pNode current, getNodeChildrenFunc getChildren,
                            getNodeValFunc getVal, freeNodeFunc freeNode, copyNodeFunc copy,
                            unsigned int best);

/**
 * @brief Compares the given subtree's best candidate against the current best candidate and
 * returns the result.
 * The function preserves the following state: no nodes on a lower level than the given root are
 * allocated, and best candidate is allocated independently of the tree.
 * @param bestCandidate - the current best candidate. The function assumes its value does not match
 * the best value.
 * @param currentChild - the root of the given subtree
 * @param getChildren - see getBest()
 * @param getVal - see getBest()
 * @param freeNode - see getBest()
 * @param best - see getBest()
 * @param copy - see getBest()
 * @return The best valued node in the tree
 * In case of an error the returned Node is NULL and bestCandidate is freed from memory.
 * If some nodes share the best valued, the function returns the first one it encounters.
 */
static pNode updateBestCandidate(pNode bestCandidate, pNode currentChild,
                                 getNodeChildrenFunc getChildren, getNodeValFunc getVal,
                                 freeNodeFunc freeNode, copyNodeFunc copy, unsigned int best)
{
    assert(bestCandidate != NULL);
    assert(getVal(bestCandidate) != best);

    pNode newCandidate;
    unsigned int bestVal = getVal(bestCandidate), newVal;

    if (currentChild == NULL) //found error
    {
        freeNode(bestCandidate);
        return NULL;
    }

    newCandidate = getSubtreeBest(currentChild, getChildren, getVal, freeNode, copy, best);
    if (newCandidate == NULL) //found error
    {
        freeNode(bestCandidate);
        return NULL;
    }

    if ((newVal = getVal(newCandidate)) > bestVal && newVal <= best)
    {
        freeNode(bestCandidate);
        return newCandidate;
    }

    freeNode(newCandidate);
    return bestCandidate;
}

/**
 * See documentation for getBest().
 */
static pNode getSubtreeBest(pNode current, getNodeChildrenFunc getChildren, getNodeValFunc getVal,
                            freeNodeFunc freeNode, copyNodeFunc copy, unsigned int best)
{
    assert(current != NULL);

    //check to see if has best value
    unsigned int val = getVal(current);
    if (val == best)
    {
        return copy(current);
    }

    //generate children list
    pNode *children;
    int numChildren = getChildren(current, &children);
    if (children == NULL)
    {
        return copy(current);
    }
    if (numChildren < 0)
    {
        free(children);
        return NULL;
    }

    //create best candidate
    pNode bestCandidate = copy(current);
    for (unsigned int i = 0; i < (unsigned int)numChildren && getVal(bestCandidate) != best; i++)
    {
        bestCandidate = updateBestCandidate(bestCandidate, children[i], getChildren,
                                            getVal, freeNode, copy, best);
        if (bestCandidate == NULL) //found error
        {
            break;
        }
    }

    //free children
    for (unsigned int i = 0; i < (unsigned int)numChildren; i++)
    {
        freeNode(children[i]);
        children[i] = NULL;
    }
    free(children);

    return bestCandidate;
}

pNode getBest(pNode head, getNodeChildrenFunc getChildren, getNodeValFunc getVal,
              freeNodeFunc freeNode, copyNodeFunc copy, unsigned int best)
{
    if (head == NULL || getChildren == NULL || getVal == NULL || freeNode == NULL || copy == NULL)
    {
        return NULL;
    }
    return getSubtreeBest(head, getChildren, getVal, freeNode, copy, best);
}
