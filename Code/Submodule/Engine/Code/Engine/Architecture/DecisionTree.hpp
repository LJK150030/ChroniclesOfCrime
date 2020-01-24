


//This file holds 

// inspired by Artificial Intelligence for Games by Ian Millington
// https://www.crcpress.com/AI-for-Games-Third-Edition/Millington/p/book/9781138483972

// base class for any decision making
template <class T>
class DecisionTreeNode
{
public:
	virtual ~DecisionTreeNode() = default;

	//function solely based on the intent of the decision node 
	virtual DecisionTreeNode* MakeADecision() = 0;
	T* m_owner;
};

// An Action to take. This is always at the end of the tree structure
template <class T>
class DTAction : public DecisionTreeNode<T>
{
public:

	virtual DecisionTreeNode<T>* MakeADecision() override
	{
		return this;
	}
};

// the nodes that make up most of the decision tree
template <class T>
class DTDecision : public DecisionTreeNode<T>
{
public:
	DecisionTreeNode<T>* m_trueBranch;
	DecisionTreeNode<T>* m_falseBranch;


	virtual bool Adjudicated() = 0;
	virtual DecisionTreeNode<T>* MakeADecision() override;
};

template <class T>
DecisionTreeNode<T>* DTDecision<T>::MakeADecision()
{
	if (Adjudicated()) {
		if (m_trueBranch == nullptr) return nullptr;
		return m_trueBranch->MakeADecision();
	}

	if (m_falseBranch == nullptr) return nullptr;
	return m_falseBranch->MakeADecision();
}
