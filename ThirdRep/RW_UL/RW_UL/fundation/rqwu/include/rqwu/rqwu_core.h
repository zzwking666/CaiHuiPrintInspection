#pragma once
#include <QString>
#include<QMap>

namespace rw {
	namespace rqwu {
		using ClassID = qint32;
		using ClassIDName = QString;
		using ModelName = QString;
		using ParamList = QMap<QString, QVariant>;
		using Index = qint32;

		//For DlgModelManager
		struct ModelInfo
		{
			ModelName name;
			ClassID classId;
			ParamList model_param_list;
		};
		using ModelInfoItem = QPair<Index, ModelInfo>;

		//For keyBoard Input
		struct Keyboard
		{
			enum class EmptyInputPolicy
			{
				EnableAndAccept,
				EnableButReject,
				DisableAndMessageBox
			};
		};
		

		void ini();
	}
}