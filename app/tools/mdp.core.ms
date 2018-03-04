DotNet.LoadAssembly "System.Xml"

struct XMLNode(
	Tag,
	Parent,
	DotNetNode,
	
	fn init dnNode = (
		DotNetNode = dnNode
		Tag = dnNode.LocalName
		Parent = dnNode.ParentNode
	),
	fn GetTag = (
		DotNetNode.Name
	),
	fn SetTag newTag =(
		try(DotNetNode.Name = newTag)catch(False)
	),
	fn GetText = (
		DotNetNode.InnerText
	), 
	fn SetText txt = (
		DotNetNode.InnerText = txt
	),
	fn AddAttribute attribute value = (
		DotNetNode.SetAttribute attribute value
	),
	fn GetAttribute attribute = (
		DotNetNode.GetAttribute attribute
	),
	fn SetAttributeValue attribute value = (
		DotNetNode.SetAttribute attribute value
	),
	fn DeleteAttribute attribute = (
		DotNetNode.RemoveAttribute attribute
	),
	fn GetAllAttributes = (
		ret = #()
		attribs = DotNetNode.Attributes
		for i = 0 to (attribs.Count - 1) do(
			t = #()
			item = (attribs.ItemOf i)
			append t item.Name
			append t item.Value
			append ret t
		)
		return ret
	),
	fn HasAttribute attribute =(
		DotNetNode.HasAttribute attribute
	),
	
	fn AppendNode newNode = (
		if classof newNode == XMLNode then(
			DotNetNode.AppendChild newNode.DotNetNode
			return True
		)else(False)
	),
	
	fn GetChild index = (
		dNode = DotNetNode.ChildNodes.ItemOf index
		if dNode.Name != "#text" then(
			newNode = XMLNode()
			newNode.init dnode
			newNode
		)else(return False)
	),
	fn GetChildren = (
		nodes = #()
		for i = 0 to (DotNetNode.ChildNodes.Count - 1) do(
			dNode = DotNetNode.ChildNodes.ItemOf i
			newNode = XMLNode()
			newNode.init dNode
			append nodes newNode
		)
		nodes
	),
	fn DeleteChild childNode = (
		if classof childNode == XMLNode then (
			DotNetNode.RemoveChild childNode.DotNetNode
			return True
		)else(False)
	)
)

struct XMLDocument ( 
	file = undefined,
	rootNode = undefined,
	dom = dotNetObject "System.Xml.XmlDocument",
	loaded = undefined,
	
	fn LoadXML pathToXml = (
		file = pathToXml
		doc = dom.Load file
		loaded = True
		True
	),
	
	fn SaveXML = (
		if loaded == True then(
			dom.Save file
			return True
		)else(False)
	),
	
	fn GetRootNode = (
		rootNode = XMLNode()
		rootNode.init dom.DocumentElement
		rootNode
	),
	fn getNewNode tag = (
		dNode=dom.CreateElement tag
		newNode=XMLNode()
		newNode.init dNode
		newNode
	),
	fn AppendNode newNode = (
		if classof newNode == XMLNode then(
			dom.AppendChild newNode.DotNetNode
			return True
		)else(False)
	),
	fn PrettySaveXML = (
		if loaded == True then(
			writer = dotNetClass "System.Xml.XmlTextWriter"
			wSettings = dotNetObject "System.Xml.XmlWriterSettings"
			wSettings.indent = True
			w = writer.create file wSettings
			--dom.writeTo w
			dom.writeContentTo w
			w.close()
			True
		)
	)
)


global MDP_OutputPath
global MDP_XML_DOC
global MDP_XML_ROOT
global MDP_XML_RESOURCES
global MDP_XML_NODES
global MDP_TopologyDependenceWarning_State_Prev

global MDP_Detector = true

global MDP_Descriptor_Name = #("Mesh", "SkinnedMesh", "TransformAnimation", "VertexAnimation", "SkeletalAnimation","Skeleton")
global MDP_Descriptor_Id = #(1001, 1002, 2001, 2002, 2003,3001)

fn MDP_GetDescriptorId desc_name = (
	return (MDP_Descriptor_Id[(findItem MDP_Descriptor_Name desc_name)])
)

global JoinIdenticalVerticesExePath = (MDP_PATH+"join_identical_vertices.exe")



fn SetTopologyDependenceWarningState state = 
(
	fn PreferenceCallbackTrue = 
	(
		fn setCheckBoxState hwnd state = 
		( 
			local BN_CLICKED = 0
			local BM_SETCHECK = 241
			local WM_COMMAND = 273

			local parent = UIAccessor.getParentWindow hwnd
			local id = UIAccessor.getWindowResourceID hwnd

			windows.sendMessage hwnd BM_SETCHECK (if state then 1 else 0) 0
			windows.sendMessage parent WM_COMMAND ((bit.shift BN_CLICKED 16) + id) hwnd 
			ok
		)
		
		hWND = DialogMonitorOPS.GetWindowHandle()

		if (UIAccessor.GetWindowText hWND) != "Preference Settings" do return true
		bt = windows.getchildhwnd hWND "Display Topology-Dependence Warning"

		setCheckBoxState bt[1] (true)

		UIAccessor.SendMessageID hWND #IDOK
		true
	)
	
	fn PreferenceCallbackFalse = 
	(
		fn setCheckBoxState hwnd state = 
		( 
			local BN_CLICKED = 0
			local BM_SETCHECK = 241
			local WM_COMMAND = 273

			local parent = UIAccessor.getParentWindow hwnd
			local id = UIAccessor.getWindowResourceID hwnd

			windows.sendMessage hwnd BM_SETCHECK (if state then 1 else 0) 0
			windows.sendMessage parent WM_COMMAND ((bit.shift BN_CLICKED 16) + id) hwnd 
			ok
		)
		
		hWND = DialogMonitorOPS.GetWindowHandle()

		if (UIAccessor.GetWindowText hWND) != "Preference Settings" do return true
		bt = windows.getchildhwnd hWND "Display Topology-Dependence Warning"

		setCheckBoxState bt[1] (false)

		UIAccessor.SendMessageID hWND #IDOK
		true
	)

	
	if (state == 1) then (
		DialogMonitorOPS.RegisterNotification PreferenceCallbackTrue id:#preferencesConfigure
	)
	if (state == 0) then (
		DialogMonitorOPS.RegisterNotification PreferenceCallbackFalse id:#preferencesConfigure
	)
	DialogMonitorOPS.Enabled = true 
	max file preferences
	DialogMonitorOPS.unRegisterNotification id:#preferencesConfigure
	DialogMonitorOPS.Enabled = false
	
)

fn TopologyDependenceWarningDisabledOn = (
	MDP_TopologyDependenceWarning_State_Prev = (getIniSetting (getMAXIniFile()) #performance #TopoMessage) as integer
	
	SetTopologyDependenceWarningState 0	
)

fn TopologyDependenceWarningDisabledOff = (
	SetTopologyDependenceWarningState MDP_TopologyDependenceWarning_State_Prev	
)


MDP_Node_Properties_GeometryType = attributes "MDP_Node_Properties_GeometryType" (
		parameters geometry_type_param rollout:geometry_type_ui
		(
			geometry_type type:#integer default:1
		)
		
		rollout geometry_type_ui "Geometry Type"
		(
			dropdownlist geometry_type_selector "Type" items:#("Node","Mesh","Skinned Mesh","Points Cloud","Path") selection:(geometry_type)
			
			on geometry_type_selector selected i do (
				geometry_type = i
			)
		)
)

MDP_Node_Properties_ExcludeFromExport = attributes "MDP_Node_Properties_ExcludeFromExport" (
		parameters exclude_from_export_param rollout:exclude_from_export_ui
		(
			exclude_from_export type:#integer default:1
		)
		
		rollout exclude_from_export_ui "Exclude From Export"
		(
			dropdownlist exclude_type "Type" items:#("Include","Exclude","Transform Only") selection:(exclude_from_export)
			
			on exclude_type selected i do (
				exclude_from_export = i
			)
		)
)


MDP_Node_Properties_Animations = attributes "MDP_Node_Properties_Animations" (
	parameters animations rollout:animations_ui
	(
		animation_name_list type:#stringTab tabSize:0 tabSizeVariable:on
		first_frame_list type:#intTab tabSize:0 tabSizeVariable:on
		last_frame_list type:#intTab tabSize:0 tabSizeVariable:on
		
		animation_name type:#string ui:animation_name_field default:"default"
		animation_first_frame type:#integer ui:animation_first_frame_field default:0
		animation_last_frame type:#integer ui:animation_last_frame_field default:1
		
		animation_type type:#integer default:1
		skeleton_root_node type:#node
	)
	
	rollout animations_ui "Animations"
	(
		dropdownlist anim_type "Type" items:#("Transform Animation", "Vertex Animation", "Skeletal Animation") selection:(animation_type)
			
		on anim_type selected i do (
			animation_type = i
		)
			
		edittext animation_name_field "Name:" text:"default" labelOnTop:true readOnly:false
		spinner animation_first_frame_field "First frame:" range:[-1000000,1000000,0] type:#integer
		spinner animation_last_frame_field "First frame:" range:[-1000000,1000000,0] type:#integer
		button animation_add "Add"
		button animation_add_from_file "Add From File"
		button animation_remove "Remove"
		button animation_remove_all "Remove All"
		listbox animations_listbox items: (for o in animation_name_list collect o)
		pickbutton skeleton_root_node_picker "Pick Skeleton Root" autoDisplay:true
		
		on animations_ui open do (
			if (skeleton_root_node != undefined) then (
				skeleton_root_node_picker.object = skeleton_root_node
				skeleton_root_node_picker.text = skeleton_root_node.name
			)
			
			if (skeleton_root_node == undefined) then (
				skeleton_root_node_picker.text = "Pick Skeleton Root"
			)
		)
		
		on skeleton_root_node_picker picked obj do (
			skeleton_root_node = obj
			skeleton_root_node_picker.object = skeleton_root_node
			skeleton_root_node_picker.text = skeleton_root_node.name
		)
			
		on animation_add pressed do (
			append animation_name_list animation_name_field.text
			append first_frame_list animation_first_frame_field.value
			append last_frame_list animation_last_frame_field.value
			
			animations_listbox.items = (for o in animation_name_list collect o)
		)
		
		on animations_listbox selected nameIndex do (
			animation_name_field.text = animation_name_list[nameIndex]
			animation_first_frame_field.value = first_frame_list[nameIndex]
			animation_last_frame_field.value = last_frame_list[nameIndex]
		)
		
		on animation_remove pressed do (
			if (animations_listbox.selection != 0) then (
				deleteItem animation_name_list animations_listbox.selection
				deleteItem first_frame_list animations_listbox.selection
				deleteItem last_frame_list animations_listbox.selection
			)
			
			animations_listbox.items = (for o in animation_name_list collect o)
		)
		
		on animation_remove_all pressed do (
			
			while (animation_name_list.count > 0) do (
				deleteItem animation_name_list 1
				deleteItem first_frame_list 1
				deleteItem last_frame_list 1
			)
			
			animations_listbox.items = (for o in animation_name_list collect o)
		)
		
		on animation_add_from_file pressed do (
			local animation_list = getOpenFileName caption:"Open Animations List File:" \
			types:"Text File(*.txt) | *.txt" \
			historyCategory:"MDP_Files"
			
			animation_list = (openFile animation_list mode:"rt")
			
			if (animation_list != undefined) then (
				
				local animations_names = #()
				local animations_first_frame = #()
				local animations_last_frame = #()
				
				while ((eof animation_list) != true) do
				(
					append animations_first_frame ((readDelimitedString animation_list " ") as integer)
					append animations_last_frame ((readDelimitedString animation_list " ") as integer)
					append animations_names (readLine animation_list)
				)
				
				close animation_list
				
				for i = 1 to animations_names.count do (
					append animation_name_list animations_names[i]
					append first_frame_list animations_first_frame[i]
					append last_frame_list animations_last_frame[i]
				)
			)
			
			animations_listbox.items = (for o in animation_name_list collect o)
		)
	)
)

fn IsEditableMesh nd = (
	print (classof nd)
	return (classof nd) == Editable_Mesh
)

fn TurnToEditableMeshOn nd = (
	if ((IsEditableMesh nd) != true) then (
		addModifier nd (Turn_To_Mesh())
	)
)

fn TurnToEditableMeshOff nd = (
	if (nd.modifiers[#Turn_to_Mesh] != undefined) then (
		deleteModifier nd nd.modifiers[#Turn_to_Mesh]
	)
)


fn MDP_ParseMesh nd parse_skin = (
	
	local skinModifier = undefined
	
	if (parse_skin == true) then (
		skinModifier = nd.modifiers[#Skin]
		
		skinModifier.enabled = false
	)
	
	clearSelection()
	select nd
	
	TurnToEditableMeshOn nd
	
	local eMod = EditNormals()
	local vertexNormalsIndexes = #{}
	local vertexNormalsIndexesArray = #()
	local averageNormal = [0,0,0]
	addModifier nd eMod
	
	local face_count = getNumFaces nd
	local uv_channel_count = (meshop.GetNumMaps nd) - 1
	
	local uv_channels_data = #()
	local vertices_data = #()
	local indices_data = #()
	local normals_data = #()
	
	local skin_vertices_index_data = #()
	
	uv_channels_data.count = uv_channel_count
	
	local i = 1
	
	for i = 1 to uv_channel_count do (
		uv_channels_data[i] = #()
	)
	
	for i = 1 to face_count do (
		for j = 1 to uv_channel_count do (
			local map_face = (meshop.getMapFace nd j i)
			local vert1 = (meshop.getMapVert nd j (map_face[1]))
			local vert2 = (meshop.getMapVert nd j (map_face[2]))
			local vert3 = (meshop.getMapVert nd j (map_face[3]))
				
			append (uv_channels_data[j]) (vert1[1])
			append (uv_channels_data[j]) (vert1[2])
				
			append (uv_channels_data[j]) (vert2[1])
			append (uv_channels_data[j]) (vert2[2])
				
			append (uv_channels_data[j]) (vert3[1])
			append (uv_channels_data[j]) (vert3[2])
		)
		
		local mesh_face = getFace nd i
		
		if (parse_skin) then (
			append skin_vertices_index_data (mesh_face[1])
			append skin_vertices_index_data (mesh_face[2])
			append skin_vertices_index_data (mesh_face[3])
		)
		
		append indices_data ((i-1)*3)
		append indices_data ((i-1)*3 + 1)
		append indices_data ((i-1)*3 + 2)
		
		local vert1 = (getVert nd (mesh_face[1]))
		local vert2 = (getVert nd (mesh_face[2]))
		local vert3 = (getVert nd (mesh_face[3]))
			
		append vertices_data (vert1[1])
		append vertices_data (vert1[2])
		append vertices_data (vert1[3])
			
		append vertices_data (vert2[1])
		append vertices_data (vert2[2])
		append vertices_data (vert2[3])
			
		append vertices_data (vert3[1])
		append vertices_data (vert3[2])
		append vertices_data (vert3[3])
			
		--vert1 = (getNormal nd (mesh_face[1]))
		--vert2 = (getNormal nd (mesh_face[2]))
		--vert3 = (getNormal nd (mesh_face[3]))
		
		vertexNormalsIndexes = #{}
		
		eMod.EditNormalsMod.ConvertVertexSelection #{mesh_face[1]} vertexNormalsIndexes
		vertexNormalsIndexesArray = (vertexNormalsIndexes as array)
		averageNormal = [0,0,0]
		for normal_index = 1 to vertexNormalsIndexesArray.count do (
			averageNormal = (averageNormal + (eMod.GetNormal (vertexNormalsIndexesArray[normal_index])))
		)

		vert1 = averageNormal / vertexNormalsIndexesArray.count
		
		vertexNormalsIndexes = #{}
		
		eMod.EditNormalsMod.ConvertVertexSelection #{mesh_face[2]} vertexNormalsIndexes
		vertexNormalsIndexesArray = (vertexNormalsIndexes as array)
		averageNormal = [0,0,0]
		for normal_index = 1 to vertexNormalsIndexesArray.count do (
			averageNormal = (averageNormal + (eMod.GetNormal (vertexNormalsIndexesArray[normal_index])))
		)
		
		vert2 = averageNormal / vertexNormalsIndexesArray.count
		
		vertexNormalsIndexes = #{}
		
		eMod.EditNormalsMod.ConvertVertexSelection #{mesh_face[3]} vertexNormalsIndexes
		vertexNormalsIndexesArray = (vertexNormalsIndexes as array)
		averageNormal = [0,0,0]
		for normal_index = 1 to vertexNormalsIndexesArray.count do (
			averageNormal = (averageNormal + (eMod.GetNormal (vertexNormalsIndexesArray[normal_index])))
		)
		
		vert3 = averageNormal / vertexNormalsIndexesArray.count
			
		append normals_data (vert1[1])
		append normals_data (vert1[2])
		append normals_data (vert1[3])
			
		append normals_data (vert2[1])
		append normals_data (vert2[2])
		append normals_data (vert2[3])
			
		append normals_data (vert3[1])
		append normals_data (vert3[2])
		append normals_data (vert3[3])
	)
	
	deleteModifier nd eMod
	TurnToEditableMeshOff nd
	
	local result = #()
	append result vertices_data
	append result uv_channels_data
	append result indices_data
	append result normals_data
	
	if (parse_skin == true) then (
		
		skinModifier.enabled = true
		
		local skin_vertex_data = #()
		local skin_vertices_index_data_count = skin_vertices_index_data.count
		
		max modify mode
		modPanel.setCurrentObject skinModifier
		
		for i = 1 to  skin_vertices_index_data_count do (
			local vert = skin_vertices_index_data[i]
			
			local vert_weights_count = (skinOps.GetVertexWeightCount skinModifier vert)
			local vert_bone_ids = #()
			local vert_bone_weights = #()
			
			for k = 1 to vert_weights_count do (
				append vert_bone_ids ((skinOps.GetVertexWeightBoneID skinModifier vert k) - 1)
				append vert_bone_weights (skinOps.GetVertexWeight skinModifier vert k)
			)
			
			append skin_vertex_data #(vert_bone_ids, vert_bone_weights)
		)
		
		local bones_names = #()
		local bones_number = (skinOps.GetNumberBones skinModifier)
		
		for i = 1 to bones_number do (
			append bones_names (skinOps.GetBoneName skinModifier i 0)
		)
		
		append result #(bones_names, skin_vertex_data)
	)
	
	return result
)

fn MDP_ParseVertexAnimationFrame nd = (
	
	clearSelection()
	select nd
	
	TurnToEditableMeshOn nd
	
	local eMod = EditNormals()
	local vertexNormalsIndexes = #{}
	local vertexNormalsIndexesArray = #()
	local averageNormal = [0,0,0]
	addModifier nd eMod
	
	local face_count = getNumFaces nd

	local vertices_data = #()
	local normals_data = #()
	
	local i = 1
	
	for i = 1 to face_count do (
		
		local mesh_face = getFace nd i
		
		local vert1 = (getVert nd (mesh_face[1]))
		local vert2 = (getVert nd (mesh_face[2]))
		local vert3 = (getVert nd (mesh_face[3]))
			
		append vertices_data (vert1[1])
		append vertices_data (vert1[2])
		append vertices_data (vert1[3])
			
		append vertices_data (vert2[1])
		append vertices_data (vert2[2])
		append vertices_data (vert2[3])
			
		append vertices_data (vert3[1])
		append vertices_data (vert3[2])
		append vertices_data (vert3[3])
			
		--vert1 = (getNormal nd (mesh_face[1]))
		--vert2 = (getNormal nd (mesh_face[2]))
		--vert3 = (getNormal nd (mesh_face[3]))
		
		vertexNormalsIndexes = #{}
		
		eMod.EditNormalsMod.ConvertVertexSelection #{mesh_face[1]} vertexNormalsIndexes
		vertexNormalsIndexesArray = (vertexNormalsIndexes as array)
		averageNormal = [0,0,0]
		for normal_index = 1 to vertexNormalsIndexesArray.count do (
			averageNormal = (averageNormal + (eMod.GetNormal (vertexNormalsIndexesArray[normal_index])))
		)

		vert1 = averageNormal / vertexNormalsIndexesArray.count
		
		vertexNormalsIndexes = #{}
		
		eMod.EditNormalsMod.ConvertVertexSelection #{mesh_face[2]} vertexNormalsIndexes
		vertexNormalsIndexesArray = (vertexNormalsIndexes as array)
		averageNormal = [0,0,0]
		for normal_index = 1 to vertexNormalsIndexesArray.count do (
			averageNormal = (averageNormal + (eMod.GetNormal (vertexNormalsIndexesArray[normal_index])))
		)
		
		vert2 = averageNormal / vertexNormalsIndexesArray.count
		
		vertexNormalsIndexes = #{}
		
		eMod.EditNormalsMod.ConvertVertexSelection #{mesh_face[3]} vertexNormalsIndexes
		vertexNormalsIndexesArray = (vertexNormalsIndexes as array)
		averageNormal = [0,0,0]
		for normal_index = 1 to vertexNormalsIndexesArray.count do (
			averageNormal = (averageNormal + (eMod.GetNormal (vertexNormalsIndexesArray[normal_index])))
		)
		
		vert3 = averageNormal / vertexNormalsIndexesArray.count
			
		append normals_data (vert1[1])
		append normals_data (vert1[2])
		append normals_data (vert1[3])
			
		append normals_data (vert2[1])
		append normals_data (vert2[2])
		append normals_data (vert2[3])
			
		append normals_data (vert3[1])
		append normals_data (vert3[2])
		append normals_data (vert3[3])
	)
	
	deleteModifier nd eMod
	TurnToEditableMeshOff nd
	
	local result = #()
	append result vertices_data
	append result normals_data
	
	return result
)

fn MDP_JoinEqualVerticesInMesh output_file = (
	dosCommand (JoinIdenticalVerticesExePath + " " + output_file)
)

fn MDP_ExportGeometry nd = (
	local type = nd.geometry_type
	
	local output_file_name = MDP_OutputPath + nd.name
	
	--Node
	if (type == 1) then (
		--do nothing
	)
	
	--Mesh
	if (type == 2) then (
		output_file_name = output_file_name + ".mesh"
		
		--local xml_mesh_node = MDP_XML_DOC.getNewNode (nd.name)
		local xml_mesh_node = MDP_XML_DOC.getNewNode ("Resource")
		xml_mesh_node.AddAttribute "Name" (nd.name)
		xml_mesh_node.AddAttribute "Type" ("Mesh")
		xml_mesh_node.AddAttribute "File" ((getFilenameFile output_file_name) + ".mesh")
		MDP_XML_RESOURCES.AppendNode xml_mesh_node
		
		close (createFile output_file_name)
		local output_file = (fopen output_file_name "wb")
		
		if (output_file != undefined) then (
			local parsed_mesh = (MDP_ParseMesh nd false)
			
			local vertices_data = parsed_mesh[1]
			local uv_channels_data = parsed_mesh[2]
			local indices_data = parsed_mesh[3]
			local normals_data = parsed_mesh[4]
			
			writelong output_file ((MDP_GetDescriptorId "Mesh") as integer) #signed
			writelong output_file ((vertices_data.count) as integer) #signed
			writelong output_file ((uv_channels_data.count) as integer) #signed
			writelong output_file (((uv_channels_data[1]).count) as integer) #signed
			writelong output_file ((indices_data.count) as integer) #signed
			writelong output_file ((normals_data.count) as integer) #signed
			
			for value in vertices_data do
			(
				writefloat output_file ((value) as float)
			)
			
			for i = 1 to uv_channels_data.count do (
				for value in (uv_channels_data[i]) do
				(
					writefloat output_file ((value) as float)
				)
			)
			
			for value in indices_data do
			(
				writelong output_file ((value) as integer) #signed
			)
			
			for value in normals_data do
			(
				writefloat output_file ((value) as float)
			)
			
			fclose output_file
			
			local option = ""
			
			if (isProperty nd "animation_type") then
			(
				--Vertex Animation
				if (nd.animation_type == 2) then (
					option = " save_vertex_join_indexes"
				)
			)
				
			MDP_JoinEqualVerticesInMesh (output_file_name + option)
		)
	)
	
	--Skinned Mesh
	if (type == 3) then (
		output_file_name = output_file_name + ".mesh"

		--local xml_mesh_node = MDP_XML_DOC.getNewNode (nd.name)
		local xml_mesh_node = MDP_XML_DOC.getNewNode ("Resource")
		xml_mesh_node.AddAttribute "Name" (nd.name)
		xml_mesh_node.AddAttribute "Type" ("SkinnedMesh")
		xml_mesh_node.AddAttribute "File" ((getFilenameFile output_file_name) + ".mesh")
		MDP_XML_RESOURCES.AppendNode xml_mesh_node
		
		close (createFile output_file_name)
		local output_file = (fopen output_file_name "wb")
		
		if (output_file != undefined) then (
			local parsed_mesh = (MDP_ParseMesh nd true)
			
			local vertices_data = parsed_mesh[1]
			local uv_channels_data = parsed_mesh[2]
			local indices_data = parsed_mesh[3]
			local normals_data = parsed_mesh[4]
			local skin_bone_names_data = parsed_mesh[5][1]
			local skin_vertex_data = parsed_mesh[5][2]
			
			writelong output_file ((MDP_GetDescriptorId "SkinnedMesh") as integer) #signed
			writelong output_file ((vertices_data.count) as integer) #signed
			writelong output_file ((uv_channels_data.count) as integer) #signed
			writelong output_file (((uv_channels_data[1]).count) as integer) #signed
			writelong output_file ((indices_data.count) as integer) #signed
			writelong output_file ((normals_data.count) as integer) #signed
			
			for value in vertices_data do
			(
				writefloat output_file ((value) as float)
			)
			
			for i = 1 to uv_channels_data.count do (
				for value in (uv_channels_data[i]) do
				(
					writefloat output_file ((value) as float)
				)
			)
			
			for value in indices_data do
			(
				writelong output_file ((value) as integer) #signed
			)
			
			for value in normals_data do
			(
				writefloat output_file ((value) as float)
			)
			
			writelong output_file ((skin_bone_names_data.count) as integer) #signed
			for bn in skin_bone_names_data do (
				writelong output_file ((bn.count + 1) as integer) #signed
				writestring output_file bn
			)
			
			writelong output_file ((skin_vertex_data.count) as integer) #signed
			
			local vxdata_count = 0
			
			for vxdata in skin_vertex_data do (
				vxdata_count = vxdata[1].count
				writelong output_file ((vxdata_count) as integer) #signed
				
				for i = 1 to vxdata_count do (
					writelong output_file ((vxdata[1][i]) as integer) #signed
					writefloat output_file ((vxdata[2][i]) as float)
				)
			)
			
			fclose output_file
			
			MDP_JoinEqualVerticesInMesh (output_file_name + " " + "skin")
		)
	)
	--Points Cloud
	if (type == 4) then (
		output_file = output_file + ".cloud"
		
		close (createFile output_file)
		output_file = (fopen output_file "wb")
		
		fclose output_file
		
	)
	
	--Path
	if (type == 5) then (
		output_file = output_file + ".path"
		
		close (createFile output_file)
		output_file = (fopen output_file "wb")
		
		fclose output_file
	)
)

fn TRSControllerCurrentValue nd = (

	local result = #()
	
	append result (nd.position.controller[1].value)
	append result (nd.position.controller[2].value)
	append result (nd.position.controller[3].value)
	
	
	if ((isProperty nd "rotation") == false) then
	(
		local rotation_part = (quatToEuler2 (nd.transform.rotationPart))
		append result (rotation_part.x)
		append result (rotation_part.y)
		append result (rotation_part.z)
	)
	else
	(
		append result (nd.rotation.controller[1].value)
		append result (nd.rotation.controller[2].value)
		append result (nd.rotation.controller[3].value)
	)
	
	append result (nd.scale.controller.value[1])
	append result (nd.scale.controller.value[2])
	append result (nd.scale.controller.value[3])
	
	
	return result
)

fn MDP_ParseLocalTransformData nd = (
	local transform_matrix = undefined
	local translation_part = undefined
	local rotation_part = undefined
	local scale_part = undefined
	
	if (nd.parent != undefined) then
	(
		transform_matrix = nd.transform * (inverse nd.parent.transform)
	)
	else
	(
		transform_matrix =nd.transform
	)
	
	translation_part = transform_matrix.translationPart
	rotation_part = quatToEuler2 (transform_matrix.rotationPart)
	scale_part = transform_matrix.scalePart
	
	return #(translation_part, rotation_part, scale_part, transform_matrix)
)

fn MDP_WriteLocalTransformDataToFile nd output_file = (
	local data = TRSControllerCurrentValue nd

	--write local TRS values
	for i = 1 to 9 do
	writefloat output_file ((data[i]) as float)
)

fn SKA_Iterate child_nd nodes_list output_file = (
	append nodes_list child_nd
	
	writelong output_file ((child_nd.name.count+1) as integer) #signed
	writestring output_file child_nd.name

	MDP_WriteLocalTransformDataToFile child_nd output_file
						
	writebyte output_file (40) #unsigned
	writelong output_file ((child_nd.children.count) as integer) #signed
	
	for child in child_nd.children do (
		SKA_Iterate child nodes_list output_file
	)
	
	writebyte output_file (41) #unsigned
)

fn MDP_ExportAnimations nd = (
	local type = nd.animation_type

	local animation = ""
	local animation_counter = 1
	local animation_first_frame = 0
	local animation_last_frame = 0
	local frame_counter = 1
	
	for animation in nd.animation_name_list do
	(
		animation_first_frame = (nd.first_frame_list[animation_counter])
		animation_last_frame = (nd.last_frame_list[animation_counter])
		frame_counter = 1
		
		local skeleton_file_name =  MDP_OutputPath + nd.name + ".skeleton"
		local skeleton_file
		
		if ((type == 1) or (type == 3)) then (
			close (createFile skeleton_file_name)
			skeleton_file = (fopen skeleton_file_name "wb")
		)
		
		
		local output_file_name = MDP_OutputPath + nd.name + "." + animation + ".animation"
		local output_file = output_file_name
		
		close (createFile output_file)
		output_file = (fopen output_file "wb")
		
		if (output_file != undefined) then (

			if (type == 1) then (
				writelong skeleton_file ((MDP_GetDescriptorId "Skeleton") as integer) #signed
				writelong output_file ((MDP_GetDescriptorId "TransformAnimation") as integer) #signed
				
				--local xml_animation_node = MDP_XML_DOC.getNewNode (nd.name+"."+animation)
				local xml_animation_node = MDP_XML_DOC.getNewNode ("Resource")
				xml_animation_node.AddAttribute "Name" (nd.name+"."+animation)
				xml_animation_node.AddAttribute "Type" ("TransformAnimation")
				xml_animation_node.AddAttribute "File" ((getFilenameFile output_file_name) + ".animation")
				MDP_XML_RESOURCES.AppendNode xml_animation_node
			)
			
			if (type == 2) then (
				writelong output_file ((MDP_GetDescriptorId "VertexAnimation") as integer) #signed
				
				--local xml_animation_node = MDP_XML_DOC.getNewNode (nd.name+"."+animation)
				local xml_animation_node = MDP_XML_DOC.getNewNode ("Resource")
				xml_animation_node.AddAttribute "Name" (nd.name+"."+animation)
				xml_animation_node.AddAttribute "Type" ("VertexAnimation")
				xml_animation_node.AddAttribute "File" ((getFilenameFile output_file_name) + ".animation")
				MDP_XML_RESOURCES.AppendNode xml_animation_node
			)
			
			if (type == 3) then (
				writelong skeleton_file ((MDP_GetDescriptorId "Skeleton") as integer) #signed
				writelong output_file ((MDP_GetDescriptorId "SkeletalAnimation") as integer) #signed
				
				--local xml_animation_node = MDP_XML_DOC.getNewNode (nd.name+"."+animation)
				local xml_animation_node = MDP_XML_DOC.getNewNode ("Resource")
				xml_animation_node.AddAttribute "Name" (nd.name+"."+animation)
				xml_animation_node.AddAttribute "Type" ("SkeletalAnimation")
				xml_animation_node.AddAttribute "File" ((getFilenameFile output_file_name) + ".animation")
				MDP_XML_RESOURCES.AppendNode xml_animation_node
			)
			
			--animation name length
			writelong output_file ((animation.count+1) as integer) #signed
			--animation name
			writestring output_file animation
			--animation duration in frames
			writelong output_file ((animation_last_frame - animation_first_frame) as integer + 1) #signed
			
			--Vertex Animation
			if (type == 2) then (
				--export delta-meshes not real meshes
				local bind_frame = undefined
				local delta_frame = undefined
				--save bind pose frame
				sliderTime = 0
				bind_frame = (MDP_ParseVertexAnimationFrame nd)
				
				local bind_vertex_data = bind_frame[1]
				local bind_normal_data = bind_frame[2]
				local vertex_count = bind_frame[1].count
				
				local delta_vertex_data = undefined
				local delta_normal_data = undefined
				
				writelong output_file ((vertex_count) as integer) #signed
				
				for frame_counter = animation_first_frame to animation_last_frame do
				(
					sliderTime = frame_counter
					
					delta_frame = (MDP_ParseVertexAnimationFrame nd)
					
					delta_vertex_data = delta_frame[1]
					delta_normal_data = delta_frame[2]

					for i = 1 to vertex_count do
					(
						writefloat output_file ((delta_vertex_data[i] - bind_vertex_data[i]) as float)
					)
					for i = 1 to vertex_count do
					(
						writefloat output_file ((delta_normal_data[i] - bind_normal_data[i]) as float)
					)
				)
			)
			
			--Skeletal Animation and Transform Animation
			if ((type == 1) or (type == 3)) then (
				
				local nodes_list = #()
				
				--save bind pose
				sliderTime = 0
				
				writebyte skeleton_file (40) #unsigned
				writelong skeleton_file ((1) as integer) #signed
				
				if (isProperty nd "skeleton_root_node") then (
					if (nd.skeleton_root_node == undefined) then
					(
						SKA_Iterate (nd) nodes_list skeleton_file	
					)
					else
					(
						SKA_Iterate (nd.skeleton_root_node) nodes_list skeleton_file
					)
				)
				else
				(
					SKA_Iterate (nd) nodes_list skeleton_file
				)
				
				writebyte skeleton_file (41) #unsigned
				
				
				local nodes_list_count = nodes_list.count
				
				writelong output_file ((nodes_list_count) as integer) #signed
				
				--for list_nd in nodes_list do (
				--	writelong skeleton_file ((list_nd.name.count+1) as integer) #signed
				--	writestring skeleton_file list_nd.name

				--	MDP_WriteLocalTransformDataToFile list_nd skeleton_file
				--)

				
				for frame_counter = animation_first_frame to animation_last_frame do
				(
					sliderTime = frame_counter
					
					for list_nd in nodes_list do (
						MDP_WriteLocalTransformDataToFile list_nd output_file
					)
				)
				
				fclose skeleton_file
			)
			
			fclose output_file
			
			--Vertex Animation
			if (type == 2) then (
				MDP_JoinEqualVerticesInMesh (output_file_name + " " + "join_vertex_animation")
			)
		)
		
		animation_counter = animation_counter + 1
	)
	
)

fn MDP_NodesCollect nd scene_graph parents_graph parent xml_parent = (
	local childrens = nd.children
	
	local data = TRSControllerCurrentValue nd
	--local xml_node = MDP_XML_DOC.getNewNode (nd.name)
	local xml_node = MDP_XML_DOC.getNewNode ("Node")
	xml_node.AddAttribute "Name" (nd.name)
	
	local xml_position_node = MDP_XML_DOC.getNewNode ("Position")
	local xml_rotation_node = MDP_XML_DOC.getNewNode ("Rotation")
	local xml_scale_node = MDP_XML_DOC.getNewNode ("Scale")
	
	xml_position_node.AddAttribute "x" (formattedPrint (data[1]) format:"#.6f")
	xml_position_node.AddAttribute "y" (formattedPrint (data[2]) format:"#.6f")
	xml_position_node.AddAttribute "z" (formattedPrint (data[3]) format:"#.6f")
	
	xml_rotation_node.AddAttribute "x" (formattedPrint (data[4]) format:"#.6f")
	xml_rotation_node.AddAttribute "y" (formattedPrint (data[5]) format:"#.6f")
	xml_rotation_node.AddAttribute "z" (formattedPrint (data[6]) format:"#.6f")
	
	xml_scale_node.AddAttribute "x" (formattedPrint (data[7]) format:"#.6f")
	xml_scale_node.AddAttribute "y" (formattedPrint (data[8]) format:"#.6f")
	xml_scale_node.AddAttribute "z" (formattedPrint (data[9]) format:"#.6f")
	
	xml_node.AppendNode xml_position_node
	xml_node.AppendNode xml_rotation_node
	xml_node.AppendNode xml_scale_node
	
	
	
	--
	local xml_resource_node = undefined
	
	if ((isProperty nd "geometry_type") == true) then (
		xml_resource_node = MDP_XML_DOC.getNewNode ("Resource")
		xml_resource_node.AddAttribute "Name" nd.name
		xml_node.AppendNode xml_resource_node
	)
	
	if ((isProperty nd "animation_type") == true) then (
		local anim_count = nd.animation_name_list.count
	
		for i = 1 to anim_count do (
			xml_resource_node = MDP_XML_DOC.getNewNode ("Resource")
			xml_resource_node.AddAttribute "Name" (nd.name + "."+(nd.animation_name_list[i]))
			xml_node.AppendNode xml_resource_node
		)
	)
	--
	
	xml_parent.AppendNode xml_node
	
	append scene_graph nd
	append parents_graph parent
	
	parent = (scene_graph.count - 1)
	for child in childrens do (
		MDP_NodesCollect child scene_graph parents_graph parent xml_node
	)
	
)

fn MDP_ExportAll = (
	local output_file_name = getSaveFileName caption:"Save To:" \
	types:"Model (*.xml) | *.xml" \
	historyCategory:"MDP_Files"
	
	--local output_file = output_file_name
	MDP_OutputPath = (getFilenamePath output_file_name)
	
	MDP_XML_DOC = XMLDocument()
	MDP_XML_DOC.file = MDP_OutputPath + (getFilenameFile output_file_name) + ".xml"
	MDP_XML_DOC.loaded = True
	MDP_XML_ROOT = MDP_XML_DOC.getNewNode "Model"
	MDP_XML_DOC.AppendNode MDP_XML_ROOT
	MDP_XML_ROOT = MDP_XML_DOC.getRootNode()
	
	MDP_XML_RESOURCES = MDP_XML_DOC.getNewNode "Resources"
	MDP_XML_ROOT.AppendNode MDP_XML_RESOURCES
	
	MDP_XML_NODES  = MDP_XML_DOC.getNewNode "Nodes"
	MDP_XML_ROOT.AppendNode MDP_XML_NODES
	--MDP_XML_MESHES = MDP_XML_DOC.getNewNode "Meshes"
	
	
	--close (createFile output_file)
	--output_file = (fopen output_file "wb")
	
	--if (output_file != undefined) then (
		
	local roots = #()
	
	for nd in objects do (
		append roots nd
	)
	
	local default_root = (Dummy pos:[0,0,0] name:"__mdp_scene_root")
	
	for o in roots do (
		if (o.parent == undefined) then (
			o.parent = default_root
		)
	)
	
	local scene_graph = #()
	local parents_graph = #()
	
	--parse scene starting from default_root
	sliderTime = 0
	MDP_NodesCollect default_root scene_graph parents_graph (scene_graph.count-1) MDP_XML_NODES
	
	--export scene graph
		
		/*local scene_graph_file_name = MDP_OutputPath + (getFilenameFile output_file_name) + ".scenegraph"
		close (createFile scene_graph_file_name)
		
		local scene_graph_file = (fopen scene_graph_file_name "wb")
		local _nd = undefined
		
		writelong scene_graph_file ((parents_graph.count) as integer) #signed
		
		for i = 1 to parents_graph.count do (
			writelong scene_graph_file ((parents_graph[i]) as integer) #signed
		)
		
		writelong scene_graph_file ((scene_graph.count) as integer) #signed
		
		for i = 1 to scene_graph.count do (
			_nd = scene_graph[i]
			
			writelong scene_graph_file ((_nd.name.count+1) as integer) #signed
			writestring scene_graph_file _nd.name
			MDP_WriteLocalTransformDataToFile _nd scene_graph_file
		)
		
		fclose scene_graph_file*/
	
	--export materials
	
	--export meshes, paths, etc
	for _nd in scene_graph do (
		if ((isProperty _nd "geometry_type") == true) then (
			MDP_ExportGeometry _nd
		)
	)

	--export animations
	for _nd in scene_graph do (

		if ((isProperty _nd "animation_name_list") == true) then (
			MDP_ExportAnimations _nd
		)
	)
	
	for o in roots do (
		if (o.parent == default_root) then (
			o.parent = undefined
		)
	)
	
	delete default_root
	
	--write model xml notation
	
	MDP_XML_DOC.SaveXML()
	MDP_XML_DOC.PrettySaveXML()
		
		--fclose output_file
	--)
)

rollout mdp_params_floater_ui "Model Data Pack - Plugin"
(
	dropdownlist lst_properties items:#("Animated Object Default Properties", "Exclude From Export") selection:(MDP_Plugin_Node.lst_property_selected)
	button btn_assign_properties "Assign Properties"
	button btn_export_all "Export Scene"
	button btn_export_subscene "Export Subscene"
	
	on lst_properties selected i do (
		MDP_Plugin_Node.lst_property_selected = i
	)
	
	on btn_assign_properties pressed do (
		
		if (MDP_Plugin_Node.lst_property_selected == 1) then (
			for nd in selection do (
				custAttributes.add nd MDP_Node_Properties_GeometryType
				custAttributes.add nd MDP_Node_Properties_Animations
			)
		)
	)
	
	on btn_export_all pressed do (
		TopologyDependenceWarningDisabledOn()
		MDP_ExportAll()
		TopologyDependenceWarningDisabledOff()
	)
	
	on btn_export_subscene pressed do (
		
	)
)