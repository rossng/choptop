<template>
	<div class="ff">
		<div v-for="(rec, index)  in recipes">
			<RecipeOverview :recipe="rec" :selected="index == selectedRecipeIndex"/>

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
				selectedRecipeIndex: 0,
			  
			}
		}
	}
</script>



<style scoped>
	h1{
	  
	}
	
</style>
