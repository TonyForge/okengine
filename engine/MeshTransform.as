package com.engine 
{
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;

	public class MeshTransform 
	{
		private static var left_matrix_operator:Matrix3D = new Matrix3D();
		private static var right_matrix_operator:Matrix3D = new Matrix3D();
		private static var rotation_operator:Matrix3D = new Matrix3D();
		
		public var rotation:Matrix3D = new Matrix3D();
		
		public var position:Vector3D = new Vector3D(0,0,0);
		public var scale:Vector3D = new Vector3D(1,1,1);
		public var euler_rotation:Vector3D = new Vector3D(0,0,0);
		
		public var forward:Vector3D = new Vector3D(1, 0, 0);
		
		public function MeshTransform() 
		{
			
		}
		
		public function copyFrom(_transform:MeshTransform):void
		{
			position.copyFrom(_transform.position);
			rotation.copyFrom(_transform.rotation);
			euler_rotation.copyFrom(_transform.euler_rotation);
			scale.copyFrom(_transform.scale);
			forward.copyFrom(_transform.forward);
		}
		
		public function CalculateForward():void
		{
			forward = rotation.transformVector(Vector3D.X_AXIS);
		}
		
		public function Reset():void
		{
			position.setTo(0, 0, 0);
			scale.setTo(1, 1, 1);
			rotation.identity();
		}
		
		public function AppendPosition(_transform:MeshTransform):void
		{
			position.x += _transform.position.x;
			position.y += _transform.position.y;
			position.z += _transform.position.z;
		}
		
		public function AppendScale(_transform:MeshTransform):void
		{
			scale.x *= _transform.scale.x;
			scale.y *= _transform.scale.y;
			scale.z *= _transform.scale.z;
		}
		
		public function AppendRotation(_transform:MeshTransform, update_euler_angles:Boolean = false):void
		{
			rotation.append(_transform.rotation);
			
			if (update_euler_angles)
			{
				euler_rotation = rotation.decompose()[2];
			}
		}
		
		public function SetToCombination_Hierarchical(parent:MeshTransform, child:MeshTransform, update_euler_angles:Boolean = false):void
		{
			var child_matrix:Matrix3D = left_matrix_operator;
			var parent_matrix:Matrix3D = right_matrix_operator;
			
			child_matrix.identity();
			child_matrix.appendScale(child.scale.x, child.scale.y, child.scale.z);
			child_matrix.append(child.rotation);
			child_matrix.appendTranslation(child.position.x, child.position.y, child.position.z);
			
			parent_matrix.identity();
			parent_matrix.appendScale(parent.scale.x, parent.scale.y, parent.scale.z);
			parent_matrix.append(parent.rotation);
			parent_matrix.appendTranslation(parent.position.x, parent.position.y, parent.position.z);
			
			child_matrix.append(parent_matrix);
			
			var decomposition:Vector.<Vector3D> = child_matrix.decompose();
			position.setTo(decomposition[0].x, decomposition[0].y, decomposition[0].z);
			scale.setTo(decomposition[2].x, decomposition[2].y, decomposition[2].z);
			
			rotation.copyFrom(child.rotation);
			rotation.append(parent.rotation);
			
			if (update_euler_angles)
			{
				euler_rotation = decomposition[1];
			}
		}
		
		public function SetToCombination(left:MeshTransform, right:MeshTransform, update_euler_angles:Boolean = false):void
		{
			position.setTo(left.position.x + right.position.x, left.position.y + right.position.y, left.position.z + right.position.z);
			scale.setTo(left.scale.x * right.scale.x, left.scale.y * right.scale.y, left.scale.z * right.scale.z);
			
			if (left != this)
			rotation.copyFrom(left.rotation);
			rotation.append(right.rotation);
			
			if (update_euler_angles)
			{
				euler_rotation = rotation.decompose()[1];
			}
		}
		
		public function Move(direction:Vector3D, speed:Number):void
		{
			position.x += direction.x * speed;
			position.y += direction.y * speed;
			position.z += direction.z * speed;
		}
		
		public function SetPosition(_position:Vector3D):void
		{
			position.setTo(_position.x, _position.y, _position.z);
		}
		
		public function SetScale(_scale:Vector3D):void
		{
			scale.setTo(_scale.x, _scale.y, _scale.z);
		}
		
		public function Rotate(x:Number, y:Number, z:Number):void
		{
			rotation_operator.identity();
			rotation_operator.appendRotation(x, Vector3D.X_AXIS);
			rotation_operator.appendRotation(y, Vector3D.Y_AXIS);
			rotation_operator.appendRotation(z, Vector3D.Z_AXIS);
			
			rotation.append(rotation_operator);
		}
		
		public function SetEulerAngles(x:Number, y:Number, z:Number):void
		{
			euler_rotation.setTo(x, y, z);
			
			rotation.identity();
			rotation.appendRotation(x, Vector3D.X_AXIS);
			rotation.appendRotation(y, Vector3D.Y_AXIS);
			rotation.appendRotation(z, Vector3D.Z_AXIS);
		}
		
		public function GetEulerAnglesX():Number
		{
			return euler_rotation.x;
		}
		
		public function GetEulerAnglesY():Number
		{
			return euler_rotation.y;
		}
		
		public function GetEulerAnglesZ():Number
		{
			return euler_rotation.z;
		}
	}

}