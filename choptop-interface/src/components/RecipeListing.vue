<template>
	<div class="ff">
		<div v-for="(rec, index)  in recipes">
			<RecipeOverview :recipe="rec" :hovered="index == hoveredRecipeIndex" :selected="index == hoveredRecipeIndex && selected"/>
		</div>
	</div>
</template>

<script>
	import RecipeOverview from './RecipeOverview'


	export default {
		name: 'recipeListing',
		components: {
			RecipeOverview
		},

		props: ['recipes'],
		data () {
			return {
				hoveredRecipeIndex: 0,
				selected: true
			  
			}
		}
	}
</script>



<style scoped>
	h1{
	
	}
	
</style>
