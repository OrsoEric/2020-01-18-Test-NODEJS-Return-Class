namespace User
{
	//Class I want to return to NODE.JS
	class My_class
	{
		public:
			//Constructor
			My_class( void );
			//Public references
			float &my_float( void );
			int &my_int( void );
		private:
			//Private class vars
			float g_my_float;
			int g_my_int;
	};
}	//End namestace: User
