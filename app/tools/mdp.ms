global MDP_PATH = "D:/ProjectsII/okengine/app/tools/"

MDP_Plugin_Node_UI = attributes "MDP_Plugin_Node" (
	parameters mdp_params rollout:mdp_params_ui
	(
		lst_property_selected type:#integer default:1
		itself_name type:#string
		mdp_path_ref type:#string
	)
	
	
	rollout mdp_params_ui "Model Data Pack - Plugin"
	(
		button btn_open_floater "Open"
		
		on mdp_params_ui open do (
			if (MDP_PATH == undefined) then (
				global MDP_PATH = mdp_path_ref
			)
			
			Execute ("global MDP_Plugin_Node = $"+itself_name)
			
			if (MDP_Detector == undefined) then (
				Execute ("filein \""+ mdp_path_ref +"mdp.core.ms\"")
			)
			
		)
		
		on btn_open_floater pressed do (
			Execute ("addRollout mdp_params_floater_ui (newRolloutFloater \"Model Data Pack - Plugin\" 300 220)")
			
		)
	)
)

custAttributes.add selection[1] MDP_Plugin_Node_UI
selection[1].itself_name = selection[1].name
selection[1].mdp_path_ref = MDP_PATH