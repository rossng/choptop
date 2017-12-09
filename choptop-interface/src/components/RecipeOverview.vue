<template>
	<div :class="getClass()">
		<div v-if="!selected">
			<h1>{{recipe.title}}</h1>
			<h2>Difficulty: {{recipe.difficulty}}</h2>
			<h2>Time: {{recipe.time}} minutes</h2>
		</div>
		<div v-if="selected && !stepsVisible">
			<h1>{{recipe.title}}</h1>
			<h2>Difficulty: {{recipe.difficulty}}</h2>
			<h2>Time: {{recipe.time}} minutes</h2>

			<div id="ingredients">
				Ingredients
				<div id="ingredientsList">
					<div v-for="(ingredient, idx) in this.recipe.ingredients" class="ingredient">
						{{getIngredientText(ingredient)}}
					</div>
				</div>
			</div>
		</div>	

		<div v-if="selected && stepsVisible">	
			<div id="steps">
				<StepDisplay :steps="recipe.steps" :stepIdx="selectedStep"/>
			</div>
		</div>

	</div>
</template>

<script>

	import StepDisplay from './StepDisplay'

	export default {
	  name: 'recipe',
	  props:['recipe', 'hovered', 'selected', "eventBus"],
	  data () {
	    return {
	      selectedStep:0,
	      focussed: false,
	      stepsVisible:false,
	    }
	  },
	  created(){
			this.eventBus.$on("pressed", this.handlePress);
	  },

	  components:{
	  	StepDisplay
	  },
	  
	  methods: {
	  	handlePress(dir){
			if (this.selected){
				if(dir == "right"){
					console.log("right step")
					this.nextStep();
				}else if(dir =="left"){
					this.prevStep();
				}else if(dir == "down"){
					console.log("steps visible")
					this.stepsVisible = true;
				}else if(dir == "up"){
					if(this.stepsVisible){
						this.stepsVisible = false;
					}else{
						this.$parent.upPressed();
					}
				}
			}
		},
	  	nextStep: function(){
	  		if(this.selectedStep < this.recipe.steps.length - 1){
	  			this.selectedStep++;
	  		}
	  	},
	  	// //Maybe should be in stepdisplay
	  	// getTime: function(index){
	  	// 	if(this.recipe.steps[index].length>1){ // How do I check if steps[i].length == 2?
	  	// 		time = this.recipe.steps[index].time;
	  	// 		return time;
	  	// 	}else{
	  	// 		return 0;
	  	// 	}
	  	// },
	  	prevStep: function(){
	  		if(this.selectedStep > 0){
	  			this.selectedStep--;
	  		}
	  	},
	  	getClass : function(){
	  		var divClass = "recipeOverview"
	  		if(this.hovered && !this.selected){
	  			divClass += " hovered"
	  		}

	  		if(this.selected){
	  			divClass += " selected"
	  		}
	  		return divClass
	  	},

	  	getIngredientText(ingredient){
	  		//this code isn't nice, sorry
	  		var text = "";
	  		if(ingredient.units.length == 2){
	  			// 
	  			if(ingredient.units[0] == "each"){
	  				var quantity = ingredient.quantity[0]
	  				text += quantity + " "
	  				text += ingredient.name
	  				if(quantity > 1 && ingredient.name[ingredient.name.length -1] != s){
	  					text += "s" //pluralise names
	  				}
	  				text += " ("+ingredient.quantity[1]+ " " + ingredient.units[1]+") "
	  			}
	  		}else {
	  			if (ingredient.units == "each"){
	  				var quantity = ingredient.quantity[0]
	  				text += quantity + " "
	  				text += ingredient.name
	  			}else if (ingredient.units == "to taste"){
	  				text += ingredient.name + " "
	  				text += ingredient.units
	  				
	  			}
	  			else{
	  				var quantity = ingredient.quantity[0]
	  				text += quantity + " " + ingredient.units+" "
	  				text += ingredient.name
	  			}
	  		}
	  		return text
	  	}
	  }
	}
</script>



<style scoped>
	.recipeOverview{
		border:1px black solid;
		margin:10px;
	}

	.recipeOverview.hovered{
		background:#ffe6bf;
	}

	.recipeOverview.selected{
		background: #68b4b2;
	}

	.ingredient{
		width:50%;
		display:inline-block;
		float:left;
	}

	#ingredientsList{
		clear:both;
		overflow:hidden;
	}
	
</style>
