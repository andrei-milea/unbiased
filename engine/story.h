#ifndef _STORY_H
#define _STORY_H

class Story
{
public:
	Story()
	{
	}

	std::vector<const std::vector<TriGram>*> get_ngrams()const
	{
		std::vector<const std::vector<TriGram>*> result;
		for(auto article_id : articles)
		{
		}
	}

private:
	std::string title_;
	std::string date_;
	std::vector<uint64_t> articles_;
	Analytics analytics_;
};

#endif

