#include "Core/Layer.h"

namespace Aine
{
	LayerStack::LayerStack()
	{
	}
	LayerStack::~LayerStack()
	{
		DestroyAll();
	}
	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layer.emplace(m_Layer.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}
	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layer.begin(), m_Layer.end(), layer);
		if (it != m_Layer.end())
		{
			m_Layer.erase(it);
			m_LayerInsertIndex--;
		}
	}
	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layer.push_back(overlay);
	}
	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layer.begin(), m_Layer.end(), overlay);
		if (it != m_Layer.end())
		{
			m_Layer.erase(it);
		}
	}
	void LayerStack::DestroyAll()
	{
		for (Layer* layer : m_Layer)
		{
			if (!layer)
				continue;

			layer->OnDestroy();
			delete layer;
		}

		m_Layer.clear();
		m_LayerInsertIndex = 0;
	}
}