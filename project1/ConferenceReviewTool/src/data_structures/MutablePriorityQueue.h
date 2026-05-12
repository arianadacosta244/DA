/*
 * MutablePriorityQueue.h
 * A simple implementation of mutable priority queues, required by Dijkstra algorithm.
 *
 * Created on: 17/03/2018
 *      Author: João Pascoal Faria
 */

#ifndef DA_TP_CLASSES_MUTABLEPRIORITYQUEUE
#define DA_TP_CLASSES_MUTABLEPRIORITYQUEUE

#include <vector>

/**
 * @brief Fila de prioridade mutável baseada em min-heap binário.
 *
 * Requer que a classe T tenha: (i) campo acessível int queueIndex; (ii) operator< definido.
 * Utilizada pelo algoritmo de Dijkstra.
 * @tparam T Tipo dos elementos (e.g. Vertex)
 * @complexity insert: O(log n), extractMin: O(log n), decreaseKey: O(log n)
 */
template <class T>
class MutablePriorityQueue {
    std::vector<T *> H;
    void heapifyUp(unsigned i);   ///< Sobe o elemento na posição i até à posição correta
    void heapifyDown(unsigned i); ///< Desce o elemento na posição i até à posição correta
    inline void set(unsigned i, T * x); ///< Define H[i] = x e atualiza queueIndex
public:
    MutablePriorityQueue();
    void insert(T * x);      ///< Insere x na fila. O(log n)
    T * extractMin();        ///< Remove e devolve o elemento mínimo. O(log n)
    void decreaseKey(T * x); ///< Atualiza a posição de x após diminuição da chave. O(log n)
    bool empty();            ///< Devolve true se a fila estiver vazia
};

// Index calculations
#define parent(i) ((i) / 2)
#define leftChild(i) ((i) * 2)

template <class T>
MutablePriorityQueue<T>::MutablePriorityQueue() {
    H.push_back(nullptr);
    // indices will be used starting in 1
    // to facilitate parent/child calculations
}

template <class T>
bool MutablePriorityQueue<T>::empty() {
    return H.size() == 1;
}

template <class T>
T* MutablePriorityQueue<T>::extractMin() {
    auto x = H[1];
    H[1] = H.back();
    H.pop_back();
    if(H.size() > 1) heapifyDown(1);
    x->queueIndex = 0;
    return x;
}

template <class T>
void MutablePriorityQueue<T>::insert(T *x) {
    H.push_back(x);
    heapifyUp(H.size()-1);
}

template <class T>
void MutablePriorityQueue<T>::decreaseKey(T *x) {
    heapifyUp(x->queueIndex);
}

template <class T>
void MutablePriorityQueue<T>::heapifyUp(unsigned i) {
    auto x = H[i];
    while (i > 1 && *x < *H[parent(i)]) {
        set(i, H[parent(i)]);
        i = parent(i);
    }
    set(i, x);
}

template <class T>
void MutablePriorityQueue<T>::heapifyDown(unsigned i) {
    auto x = H[i];
    while (true) {
        unsigned k = leftChild(i);
        if (k >= H.size())
            break;
        if (k+1 < H.size() && *H[k+1] < *H[k])
            ++k; // right child of i
        if ( ! (*H[k] < *x) )
            break;
        set(i, H[k]);
        i = k;
    }
    set(i, x);
}

template <class T>
void MutablePriorityQueue<T>::set(unsigned i, T * x) {
    H[i] = x;
    x->queueIndex = i;
}

#endif /* DA_TP_CLASSES_MUTABLEPRIORITYQUEUE */